// Copyright (c) 2011 The LevelDB Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.
//
// WriteBatch::rep_ :=
//    sequence: fixed64
//    count: fixed32
//    data: record[count]
// record :=
//    kTypeValue varstring varstring         |
//    kTypeDeletion varstring
// varstring :=
//    len: varint32
//    data: uint8[len]

#include "leveldb/write_batch.h"

#include "db/dbformat.h"
#include "db/memtable.h"
#include "db/write_batch_internal.h"
#include "leveldb/db.h"
#include "util/coding.h"

namespace leveldb {

    // WriteBatch 标头具有 8 字节的序列号，后跟 4 字节的计数。
    static const size_t kHeader = 12;

    WriteBatch::WriteBatch() { Clear(); }

    WriteBatch::~WriteBatch() = default;

    WriteBatch::Handler::~Handler() = default;

    void WriteBatch::Clear() {
        rep_.clear();
        rep_.resize(kHeader);
    }

    size_t WriteBatch::ApproximateSize() const { return rep_.size(); }

    /**
     * 迭代器
     * @param handler {@link MemTableInserter}
     * @return
     */
    Status WriteBatch::Iterate(Handler *handler) const {
        Slice input(rep_);
        // 如果给 Slice 分配的空间太小不到 12 字节
        if (input.size() < kHeader) {
            return Status::Corruption("malformed WriteBatch (too small)");
        }

        // 标记已经使用的字节大小
        input.remove_prefix(kHeader);
        Slice key, value;
        int found = 0;
        while (!input.empty()) {
            found++;
            char tag = input[0];
            input.remove_prefix(1);
            switch (tag) {
                case kTypeValue: // 插入标识
                    if (GetLengthPrefixedSlice(&input, &key) && GetLengthPrefixedSlice(&input, &value)) {
                        // 目前知道是通过 MemTableInserter 将 key->value 添加到 mem 中
                        handler->Put(key, value);
                    } else {
                        return Status::Corruption("bad WriteBatch Put");
                    }
                    break;
                case kTypeDeletion: // 删除标识
                    if (GetLengthPrefixedSlice(&input, &key)) {
                        handler->Delete(key);
                    } else {
                        return Status::Corruption("bad WriteBatch Delete");
                    }
                    break;
                default:
                    return Status::Corruption("unknown WriteBatch tag");
            }
        }
        if (found != WriteBatchInternal::Count(this)) {
            // WriteBatch 计数错误
            return Status::Corruption("WriteBatch has wrong count");
        } else {
            return Status::OK();
        }
    }

    int WriteBatchInternal::Count(const WriteBatch *b) {
        return DecodeFixed32(b->rep_.data() + 8);
    }

    void WriteBatchInternal::SetCount(WriteBatch *b, int n) {
        EncodeFixed32(&b->rep_[8], n);
    }

    SequenceNumber WriteBatchInternal::Sequence(const WriteBatch *b) {
        return SequenceNumber(DecodeFixed64(b->rep_.data()));
    }

    void WriteBatchInternal::SetSequence(WriteBatch *b, SequenceNumber seq) {
        EncodeFixed64(&b->rep_[0], seq);
    }

    /**
     * 插入一条数据
     * @param key
     * @param value
     */
    void WriteBatch::Put(const Slice &key, const Slice &value) {
        WriteBatchInternal::SetCount(this, WriteBatchInternal::Count(this) + 1);
        rep_.push_back(static_cast<char>(kTypeValue)); // 将 value 添加到 rep_ 末尾
        PutLengthPrefixedSlice(&rep_, key);
        PutLengthPrefixedSlice(&rep_, value);
    }

    void WriteBatch::Delete(const Slice &key) {
        WriteBatchInternal::SetCount(this, WriteBatchInternal::Count(this) + 1);
        rep_.push_back(static_cast<char>(kTypeDeletion));
        PutLengthPrefixedSlice(&rep_, key);
    }

    void WriteBatch::Append(const WriteBatch &source) {
        WriteBatchInternal::Append(this, &source);
    }

    namespace {
        /** 内存表插入器 */
        class MemTableInserter : public WriteBatch::Handler {
        public:
            SequenceNumber sequence_;
            MemTable *mem_;

            void Put(const Slice &key, const Slice &value) override {
                // 添加到内存中
                mem_->Add(sequence_, kTypeValue, key, value);
                sequence_++;
            }

            void Delete(const Slice &key) override {
                mem_->Add(sequence_, kTypeDeletion, key, Slice());
                sequence_++;
            }
        };
    }  // namespace

    Status WriteBatchInternal::InsertInto(const WriteBatch *b, MemTable *memtable) {
        MemTableInserter inserter;
        inserter.sequence_ = WriteBatchInternal::Sequence(b);
        inserter.mem_ = memtable;
        return b->Iterate(&inserter);
    }

    void WriteBatchInternal::SetContents(WriteBatch *b, const Slice &contents) {
        assert(contents.size() >= kHeader);
        b->rep_.assign(contents.data(), contents.size());
    }

    void WriteBatchInternal::Append(WriteBatch *dst, const WriteBatch *src) {
        SetCount(dst, Count(dst) + Count(src));
        assert(src->rep_.size() >= kHeader);
        dst->rep_.append(src->rep_.data() + kHeader, src->rep_.size() - kHeader);
    }

}  // namespace leveldb
