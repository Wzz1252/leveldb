// Copyright (c) 2011 The LevelDB Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.
//
// Log format information shared by reader and writer.
// See ../doc/log_format.md for more detail.

#ifndef STORAGE_LEVELDB_DB_LOG_FORMAT_H_
#define STORAGE_LEVELDB_DB_LOG_FORMAT_H_

namespace leveldb {
    namespace log {

        /**
         * 记录类型
         */
        enum RecordType {
            /** 零保留给预分配的文件 */
            kZeroType = 0,
            /** 记录完全在一个 block 中 */
            kFullType = 1,
            /** 当前 block 容纳不下所有的内容，记录的第一片在本 block 中 */
            kFirstType = 2,
            /** 记录的内容的起始位置不在本 block，结束位置也不在本 block */
            kMiddleType = 3,
            /** 记录的内容起始位置不在本 block，但结束位置在本 block */
            kLastType = 4
        };
        static const int kMaxRecordType = kLastType;

        /** 字节，32KB，一个块的大小 */
        static const int kBlockSize = 32768;

        /**
         * 头的大小： 7 B
         *  checksum：4个字节，是对"类型"和"数据"字段的校验码，为了避免处理不完整或者是被破坏的数据。
         *  length：2个字节，表示 payload 的长度
         *  type：1个字节，指出了每条 log record 和 log block 之间的结构关系，详细看 RecordType
         *
         *                      Block 32KB
         * |---------------------------------------------------------|
         * |        4        |    2    |  1  |        content        |
         * |---------------------------------------------------------|
         *       checksum      length   type       data
         *
         * checksum: uint32         type 及 data[] 对应的 crc32 值
         * length:   uint16         数据的长度
         * type:     uint8          FULL/FIRST/MIDDLE/LAST 中的一种
         * data:     uint8[length]  实际存储的数据
         */
        static const int kHeaderSize = 4 + 2 + 1;

    }  // namespace log
}  // namespace leveldb

#endif  // STORAGE_LEVELDB_DB_LOG_FORMAT_H_
