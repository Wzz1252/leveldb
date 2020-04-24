// Copyright (c) 2011 The LevelDB Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.
//
// The representation of a DBImpl consists of a set of Versions.  The
// newest version is called "current".  Older versions may be kept
// around to provide a consistent view to live iterators.
//
// Each Version keeps track of a set of Table files per level.  The
// entire set of versions is maintained in a VersionSet.
//
// Version,VersionSet are thread-compatible, but require external
// synchronization on all accesses.

#ifndef STORAGE_LEVELDB_DB_VERSION_SET_H_
#define STORAGE_LEVELDB_DB_VERSION_SET_H_

#include <map>
#include <set>
#include <vector>

#include "db/dbformat.h"
#include "db/version_edit.h"
#include "port/port.h"
#include "port/thread_annotations.h"

namespace leveldb {

    namespace log {
        class Writer;
    }

    class Compaction;

    class Iterator;

    class MemTable;

    class TableBuilder;

    class TableCache;

    class Version;

    class VersionSet;

    class WritableFile;

    // Return the smallest index i such that files[i]->largest >= key.
    // Return files.size() if there is no such file.
    // REQUIRES: "files" contains a sorted list of non-overlapping files.
    int FindFile(const InternalKeyComparator &icmp, const std::vector<FileMetaData *> &files, const Slice &key);

    // Returns true iff some file in "files" overlaps the user key range
    // [*smallest,*largest].
    // smallest==nullptr represents a key smaller than all keys in the DB.
    // largest==nullptr represents a key largest than all keys in the DB.
    // REQUIRES: If disjoint_sorted_files, files[] contains disjoint ranges
    //           in sorted order.
    bool SomeFileOverlapsRange(const InternalKeyComparator &icmp,
                               bool disjoint_sorted_files,
                               const std::vector<FileMetaData *> &files,
                               const Slice *smallest_user_key,
                               const Slice *largest_user_key);

    class Version {
    public:
        // Lookup the value for key.  If found, store it in *val and
        // return OK.  Else return a non-OK status.  Fills *stats.
        // REQUIRES: lock is not held
        struct GetStats {
            FileMetaData *seek_file;
            int seek_file_level;
        };

        // Append to *iters a sequence of iterators that will
        // yield the contents of this Version when merged together.
        // REQUIRES: This version has been saved (see VersionSet::SaveTo)
        void AddIterators(const ReadOptions &, std::vector<Iterator *> *iters);

        Status Get(const ReadOptions &, const LookupKey &key, std::string *val,
                   GetStats *stats);

        // Adds "stats" into the current state.  Returns true if a new
        // compaction may need to be triggered, false otherwise.
        // REQUIRES: lock is held
        bool UpdateStats(const GetStats &stats);

        // Record a sample of bytes read at the specified internal key.
        // Samples are taken approximately once every config::kReadBytesPeriod
        // bytes.  Returns true if a new compaction may need to be triggered.
        // REQUIRES: lock is held
        bool RecordReadSample(Slice key);

        // Reference count management (so Versions do not disappear out from
        // under live iterators)
        void Ref();

        void Unref();

        void GetOverlappingInputs(
                int level,
                const InternalKey *begin,  // nullptr means before all keys
                const InternalKey *end,    // nullptr means after all keys
                std::vector<FileMetaData *> *inputs);

        // Returns true iff some file in the specified level overlaps
        // some part of [*smallest_user_key,*largest_user_key].
        // smallest_user_key==nullptr represents a key smaller than all the DB's keys.
        // largest_user_key==nullptr represents a key largest than all the DB's keys.
        bool OverlapInLevel(int level, const Slice *smallest_user_key,
                            const Slice *largest_user_key);

        // Return the level at which we should place a new memtable compaction
        // result that covers the range [smallest_user_key,largest_user_key].
        int PickLevelForMemTableOutput(const Slice &smallest_user_key,
                                       const Slice &largest_user_key);

        int NumFiles(int level) const { return files_[level].size(); }

        // Return a human readable string that describes this version's contents.
        std::string DebugString() const;

    private:
        friend class Compaction;

        friend class VersionSet;

        class LevelFileNumIterator;

        explicit Version(VersionSet *vset)
                : vset_(vset),
                  next_(this),
                  prev_(this),
                  refs_(0),
                  file_to_compact_(nullptr),
                  file_to_compact_level_(-1),
                  compaction_score_(-1),
                  compaction_level_(-1) {}

        Version(const Version &) = delete;

        Version &operator=(const Version &) = delete;

        ~Version();

        Iterator *NewConcatenatingIterator(const ReadOptions &, int level) const;

        // Call func(arg, level, f) for every file that overlaps user_key in
        // order from newest to oldest.  If an invocation of func returns
        // false, makes no more calls.
        //
        // REQUIRES: user portion of internal_key == user_key.
        void ForEachOverlapping(Slice user_key, Slice internal_key, void *arg,
                                bool (*func)(void *, int, FileMetaData *));

        VersionSet *vset_;  // 该版本所属的 VersionSet
        Version *next_;     // 链表中的下一个版本
        Version *prev_;     // 链表中的上一个版本
        int refs_;          // 该版本存活的引用数量

        // 每个级别的文件列表
        std::vector<FileMetaData *> files_[config::kNumLevels];

        // 根据搜索统计信息压缩的下一个文件
        FileMetaData *file_to_compact_;
        int file_to_compact_level_;

        // 接下来应压缩的级别及其压缩分数。
        // 得分<1 表示不需要严格压缩。这些字段由 Finalize() 初始化。
        double compaction_score_;
        int compaction_level_;
    };

    class VersionSet {
    public:
        VersionSet(const std::string &dbname, const Options *options, TableCache *table_cache,
                   const InternalKeyComparator *);

        VersionSet(const VersionSet &) = delete;

        VersionSet &operator=(const VersionSet &) = delete;

        ~VersionSet();

        // 将 *edit 应用于当前版本以形成一个新的描述符，该描述符既保存到持久状态又安装为新的当前版本。在实际写入文件时将释放 *mu
        // 要求：*mu 在进入时被保留。
        // 要求：没有其他线程同时调用 LogAndApply()。
        Status LogAndApply(VersionEdit *edit, port::Mutex *mu) EXCLUSIVE_LOCKS_REQUIRED(mu);

        /** 从持久性存储中恢复最后保存的描述符 */
        Status Recover(bool *save_manifest);

        /** 返回当前版本号 */
        Version *current() const { return current_; }

        // Return the current manifest file number
        uint64_t ManifestFileNumber() const { return manifest_file_number_; }

        // 分配并返回新的文件号
        uint64_t NewFileNumber() { return next_file_number_++; }

        // Arrange to reuse "file_number" unless a newer file number has
        // already been allocated.
        // REQUIRES: "file_number" was returned by a call to NewFileNumber().
        void ReuseFileNumber(uint64_t file_number) {
            if (next_file_number_ == file_number + 1) {
                next_file_number_ = file_number;
            }
        }

        // Return the number of Table files at the specified level.
        int NumLevelFiles(int level) const;

        // Return the combined file size of all files at the specified level.
        int64_t NumLevelBytes(int level) const;

        // 返回最后一个序列号
        uint64_t LastSequence() const { return last_sequence_; }

        // Set the last sequence number to s.
        void SetLastSequence(uint64_t s) {
            assert(s >= last_sequence_);
            last_sequence_ = s;
        }

        // Mark the specified file number as used.
        void MarkFileNumberUsed(uint64_t number);

        // Return the current log file number.
        uint64_t LogNumber() const { return log_number_; }

        // Return the log file number for the log file that is currently
        // being compacted, or zero if there is no such log file.
        uint64_t PrevLogNumber() const { return prev_log_number_; }

        // Pick level and inputs for a new compaction.
        // Returns nullptr if there is no compaction to be done.
        // Otherwise returns a pointer to a heap-allocated object that
        // describes the compaction.  Caller should delete the result.
        Compaction *PickCompaction();

        // Return a compaction object for compacting the range [begin,end] in
        // the specified level.  Returns nullptr if there is nothing in that
        // level that overlaps the specified range.  Caller should delete
        // the result.
        Compaction *CompactRange(int level, const InternalKey *begin, const InternalKey *end);

        // Return the maximum overlapping data (in bytes) at next level for any
        // file at a level >= 1.
        int64_t MaxNextLevelOverlappingBytes();

        // Create an iterator that reads over the compaction inputs for "*c".
        // The caller should delete the iterator when no longer needed.
        Iterator *MakeInputIterator(Compaction *c);

        // 如果某个级别需要压缩，则返回 true
        bool NeedsCompaction() const {
            Version *v = current_;
            return (v->compaction_score_ >= 1) || (v->file_to_compact_ != nullptr);
        }

        // Add all files listed in any live version to *live.
        // May also mutate some internal state.
        void AddLiveFiles(std::set<uint64_t> *live);

        // Return the approximate offset in the database of the data for
        // "key" as of version "v".
        uint64_t ApproximateOffsetOf(Version *v, const InternalKey &key);

        // Return a human-readable short (single-line) summary of the number
        // of files per level.  Uses *scratch as backing store.
        struct LevelSummaryStorage {
            char buffer[100];
        };

        const char *LevelSummary(LevelSummaryStorage *scratch) const;

    private:
        class Builder;

        friend class Compaction;

        friend class Version;

        bool ReuseManifest(const std::string &dscname, const std::string &dscbase);

        void Finalize(Version *v);

        void GetRange(const std::vector<FileMetaData *> &inputs,
                      InternalKey *smallest,
                      InternalKey *largest);

        void GetRange2(const std::vector<FileMetaData *> &inputs1,
                       const std::vector<FileMetaData *> &inputs2,
                       InternalKey *smallest, InternalKey *largest);

        void SetupOtherInputs(Compaction *c);

        // Save current contents to *log
        Status WriteSnapshot(log::Writer *log);

        void AppendVersion(Version *v);

        Env *const env_;
        const std::string dbname_;
        const Options *const options_;
        TableCache *const table_cache_;
        const InternalKeyComparator icmp_;
        uint64_t next_file_number_;
        uint64_t manifest_file_number_;
        uint64_t last_sequence_;
        uint64_t log_number_;
        uint64_t prev_log_number_;  // 0 or backing store for memtable being compacted

        // Opened lazily
        WritableFile *descriptor_file_;
        log::Writer *descriptor_log_;
        /** 循环双链接版本列表的头 */
        Version dummy_versions_;
        Version *current_;        // == dummy_versions_.prev_

        // Per-level key at which the next compaction at that level should start.
        // Either an empty string, or a valid InternalKey.
        std::string compact_pointer_[config::kNumLevels];
    };

    // A Compaction encapsulates information about a compaction.
    class Compaction {
    public:
        ~Compaction();

        // Return the level that is being compacted.  Inputs from "level"
        // and "level+1" will be merged to produce a set of "level+1" files.
        int level() const { return level_; }

        // Return the object that holds the edits to the descriptor done
        // by this compaction.
        VersionEdit *edit() { return &edit_; }

        // "which" must be either 0 or 1
        int num_input_files(int which) const { return inputs_[which].size(); }

        // Return the ith input file at "level()+which" ("which" must be 0 or 1).
        FileMetaData *input(int which, int i) const { return inputs_[which][i]; }

        // Maximum size of files to build during this compaction.
        uint64_t MaxOutputFileSize() const { return max_output_file_size_; }

        // Is this a trivial compaction that can be implemented by just
        // moving a single input file to the next level (no merging or splitting)
        bool IsTrivialMove() const;

        // Add all inputs to this compaction as delete operations to *edit.
        void AddInputDeletions(VersionEdit *edit);

        // Returns true if the information we have available guarantees that
        // the compaction is producing data in "level+1" for which no data exists
        // in levels greater than "level+1".
        bool IsBaseLevelForKey(const Slice &user_key);

        // Returns true iff we should stop building the current output
        // before processing "internal_key".
        bool ShouldStopBefore(const Slice &internal_key);

        // Release the input version for the compaction, once the compaction
        // is successful.
        void ReleaseInputs();

    private:
        friend class Version;

        friend class VersionSet;

        Compaction(const Options *options, int level);

        int level_;
        uint64_t max_output_file_size_;
        Version *input_version_;
        VersionEdit edit_;

        // Each compaction reads inputs from "level_" and "level_+1"
        std::vector<FileMetaData *> inputs_[2];  // The two sets of inputs

        // State used to check for number of overlapping grandparent files
        // (parent == level_ + 1, grandparent == level_ + 2)
        std::vector<FileMetaData *> grandparents_;
        size_t grandparent_index_;  // Index in grandparent_starts_
        bool seen_key_;             // Some output key has been seen
        int64_t overlapped_bytes_;  // Bytes of overlap between current output
        // and grandparent files

        // State for implementing IsBaseLevelForKey

        // level_ptrs_ holds indices into input_version_->levels_: our state
        // is that we are positioned at one of the file ranges for each
        // higher level than the ones involved in this compaction (i.e. for
        // all L >= level_ + 2).
        size_t level_ptrs_[config::kNumLevels];
    };

}  // namespace leveldb

#endif  // STORAGE_LEVELDB_DB_VERSION_SET_H_
