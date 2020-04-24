// Copyright (c) 2011 The LevelDB Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#ifndef STORAGE_LEVELDB_DB_WRITE_BATCH_INTERNAL_H_
#define STORAGE_LEVELDB_DB_WRITE_BATCH_INTERNAL_H_

#include "db/dbformat.h"
#include "leveldb/write_batch.h"

namespace leveldb {

    class MemTable;

    // WriteBatchInternal 提供了静态方法来处理我们在公共 WriteBatch 接口中不需要的 WriteBatch。
    class WriteBatchInternal {
    public:
        // 返回批处理中的条目数
        static int Count(const WriteBatch *batch);

        // 设置批处理中条目数的计数
        static void SetCount(WriteBatch *batch, int n);

        // 返回批处理开始的序列号
        static SequenceNumber Sequence(const WriteBatch *batch);

        // 将指定的编号存储为此批处理开始的序列号
        static void SetSequence(WriteBatch *batch, SequenceNumber seq);

        static Slice Contents(const WriteBatch *batch) { return Slice(batch->rep_); }

        static size_t ByteSize(const WriteBatch *batch) { return batch->rep_.size(); }

        static void SetContents(WriteBatch *batch, const Slice &contents);

        static Status InsertInto(const WriteBatch *batch, MemTable *memtable);

        static void Append(WriteBatch *dst, const WriteBatch *src);
    };

}  // namespace leveldb

#endif  // STORAGE_LEVELDB_DB_WRITE_BATCH_INTERNAL_H_
