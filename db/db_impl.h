// Copyright (c) 2011 The LevelDB Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#ifndef STORAGE_LEVELDB_DB_DB_IMPL_H_
#define STORAGE_LEVELDB_DB_DB_IMPL_H_

#include <atomic>
#include <deque>
#include <set>
#include <string>

#include "db/dbformat.h"
#include "db/log_writer.h"
#include "db/snapshot.h"
#include "leveldb/db.h"
#include "leveldb/env.h"
#include "port/port.h"
#include "port/thread_annotations.h"

namespace leveldb {

    class MemTable;

    class TableCache;

    class Version;

    class VersionEdit;

    class VersionSet;

    /**
     * DB 的具体实现类
     */
    class DBImpl : public DB {
    public:
        DBImpl(const Options &options, const std::string &dbname);

        DBImpl(const DBImpl &) = delete;

        DBImpl &operator=(const DBImpl &) = delete;

        ~DBImpl() override;

        // 数据库接口的实现
        Status Put(const WriteOptions &, const Slice &key, const Slice &value) override;

        Status Delete(const WriteOptions &, const Slice &key) override;

        Status Write(const WriteOptions &options, WriteBatch *updates) override;

        Status Get(const ReadOptions &options, const Slice &key, std::string *value) override;

        Iterator *NewIterator(const ReadOptions &) override;

        const Snapshot *GetSnapshot() override;

        void ReleaseSnapshot(const Snapshot *snapshot) override;

        bool GetProperty(const Slice &property, std::string *value) override;

        void GetApproximateSizes(const Range *range, int n, uint64_t *sizes) override;

        void CompactRange(const Slice *begin, const Slice *end) override;

        // 公有 DB 接口中没有的其他方法（用于测试）

        // Compact any files in the named level that overlap [*begin,*end]
        void TEST_CompactRange(int level, const Slice *begin, const Slice *end);

        // Force current memtable contents to be compacted.
        Status TEST_CompactMemTable();

        // Return an internal iterator over the current state of the database.
        // The keys of this iterator are internal keys (see format.h).
        // The returned iterator should be deleted when no longer needed.
        Iterator *TEST_NewInternalIterator();

        // Return the maximum overlapping data (in bytes) at next level for any
        // file at a level >= 1.
        int64_t TEST_MaxNextLevelOverlappingBytes();

        // Record a sample of bytes read at the specified internal key.
        // Samples are taken approximately once every config::kReadBytesPeriod
        // bytes.
        void RecordReadSample(Slice key);

    private:
        friend class DB;

        struct CompactionState;
        struct Writer;

        // Information for a manual compaction
        struct ManualCompaction {
            int level;
            bool done;
            const InternalKey *begin;  // null means beginning of key range
            const InternalKey *end;    // null means end of key range
            InternalKey tmp_storage;   // Used to keep track of compaction progress
        };

        // Per level compaction stats.  stats_[level] stores the stats for
        // compactions that produced data for the specified "level".
        struct CompactionStats {
            CompactionStats() : micros(0), bytes_read(0), bytes_written(0) {}

            void Add(const CompactionStats &c) {
                this->micros += c.micros;
                this->bytes_read += c.bytes_read;
                this->bytes_written += c.bytes_written;
            }

            int64_t micros;
            int64_t bytes_read;
            int64_t bytes_written;
        };

        Iterator *NewInternalIterator(const ReadOptions &, SequenceNumber *latest_snapshot, uint32_t *seed);

        Status NewDB();

        // Recover the descriptor from persistent storage.  May do a significant amount of work to recover recently logged updates.  Any changes to be made to the descriptor are added to *edit.
        // 从持久性存储中恢复描述符。可能需要做大量工作才能恢复最近记录的更新。对描述符所做的任何更改都会添加到 *edit 中。
        Status Recover(VersionEdit *edit, bool *save_manifest) EXCLUSIVE_LOCKS_REQUIRED(mutex_);

        void MaybeIgnoreError(Status *s) const;

        // Delete any unneeded files and stale in-memory entries.
        void RemoveObsoleteFiles() EXCLUSIVE_LOCKS_REQUIRED(mutex_);

        // 将内存中的写缓冲区压缩到磁盘。切换到新的 日志文件/内存表 并成功写入新的描述符。
        // 错误记录在 bg_error_ 中
        void CompactMemTable() EXCLUSIVE_LOCKS_REQUIRED(mutex_);

        Status RecoverLogFile(uint64_t log_number, bool last_log, bool *save_manifest,
                              VersionEdit *edit, SequenceNumber *max_sequence)
        EXCLUSIVE_LOCKS_REQUIRED(mutex_);

        Status WriteLevel0Table(MemTable *mem, VersionEdit *edit, Version *base)
        EXCLUSIVE_LOCKS_REQUIRED(mutex_);

        Status MakeRoomForWrite(bool force /* compact even if there is room? */)
        EXCLUSIVE_LOCKS_REQUIRED(mutex_);

        WriteBatch *BuildBatchGroup(Writer **last_writer)
        EXCLUSIVE_LOCKS_REQUIRED(mutex_);

        void RecordBackgroundError(const Status &s);

        void MaybeScheduleCompaction() EXCLUSIVE_LOCKS_REQUIRED(mutex_);

        static void BGWork(void *db);

        void BackgroundCall();

        /** 后台压缩 */
        void BackgroundCompaction() EXCLUSIVE_LOCKS_REQUIRED(mutex_);

        void CleanupCompaction(CompactionState *compact) EXCLUSIVE_LOCKS_REQUIRED(mutex_);

        Status DoCompactionWork(CompactionState *compact) EXCLUSIVE_LOCKS_REQUIRED(mutex_);

        Status OpenCompactionOutputFile(CompactionState *compact);

        Status FinishCompactionOutputFile(CompactionState *compact, Iterator *input);

        Status InstallCompactionResults(CompactionState *compact) EXCLUSIVE_LOCKS_REQUIRED(mutex_);

        /**
         * 用户比较器
         */
        const Comparator *user_comparator() const {
            return internal_comparator_.user_comparator();
        }

        // Constant after construction
        // env 是系统环境相关
        Env *const env_;

        const InternalKeyComparator internal_comparator_;
        const InternalFilterPolicy internal_filter_policy_;

        const Options options_;  // options_.comparator == &internal_comparator_
        const bool owns_info_log_;
        const bool owns_cache_;
        const std::string dbname_;

        // table_cache_ provides its own synchronization
        TableCache *const table_cache_;

        // 锁定持久数据库状态。成功获取 非null 的 iff。
        FileLock *db_lock_;

        // 以下状态受互斥锁保护
        port::Mutex mutex_;
        std::atomic<bool> shutting_down_; // 关机
        port::CondVar background_work_finished_signal_ GUARDED_BY(mutex_);
        MemTable *mem_; // 原始内存数据
        MemTable *imm_ GUARDED_BY(mutex_);  // Memtable 被压缩后的
        std::atomic<bool> has_imm_;         // So bg thread can detect non-null imm_
        WritableFile *logfile_;
        uint64_t logfile_number_ GUARDED_BY(mutex_);
        log::Writer *log_;
        uint32_t seed_ GUARDED_BY(mutex_);  // 采样率

        // Queue of writers.
        std::deque<Writer *> writers_ GUARDED_BY(mutex_);
        WriteBatch *tmp_batch_ GUARDED_BY(mutex_);

        SnapshotList snapshots_ GUARDED_BY(mutex_);

        // 防止被删除的表文件集，因为它们是正在进行的压缩的一部分。
        std::set<uint64_t> pending_outputs_ GUARDED_BY(mutex_);

        // 是否已安排后台压缩或正在运行后台压缩？
        bool background_compaction_scheduled_ GUARDED_BY(mutex_);

        // 手动压缩
        ManualCompaction *manual_compaction_ GUARDED_BY(mutex_);

        VersionSet *const versions_ GUARDED_BY(mutex_);

        // 在偏执模式下是否遇到后台错误？
        Status bg_error_ GUARDED_BY(mutex_);

        CompactionStats stats_[config::kNumLevels] GUARDED_BY(mutex_);
    };

    /**
     * 清理数据库选项。如果不等于 src.info_log，则调用方应删除 result.info_log。
     * @param db
     * @param icmp
     * @param ipolicy
     * @param src
     * @return
     */
    Options SanitizeOptions(const std::string &db,
                            const InternalKeyComparator *icmp,
                            const InternalFilterPolicy *ipolicy,
                            const Options &src);

}  // namespace leveldb

#endif  // STORAGE_LEVELDB_DB_DB_IMPL_H_
