// Copyright (c) 2011 The LevelDB Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#include "db/log_writer.h"

#include <stdint.h>

#include "leveldb/env.h"
#include "util/coding.h"
#include "util/crc32c.h"

namespace leveldb {
    namespace log {

        static void InitTypeCrc(uint32_t *type_crc) {
            for (int i = 0; i <= kMaxRecordType; i++) {
                char t = static_cast<char>(i);
                type_crc[i] = crc32c::Value(&t, 1);
            }
        }

        Writer::Writer(WritableFile *dest) : dest_(dest), block_offset_(0) {
            InitTypeCrc(type_crc_);
        }

        Writer::Writer(WritableFile *dest, uint64_t dest_length)
                : dest_(dest), block_offset_(dest_length % kBlockSize) {
            InitTypeCrc(type_crc_);
        }

        Writer::~Writer() = default;

        /**
         * 用于对日志的添加记录
         * 此方法是对字符串的拼接
         * 日志在 leveldb 中是以块（Slice）的形式存在的，每个块的长度都是 32KB，leveldb 通过 RecordType 来标记当前记录在块中的位置。
         * @param slice
         * @return
         */
        Status Writer::AddRecord(const Slice &slice) {
            const char *ptr = slice.data();
            size_t left = slice.size();

            // 如有必要，将记录分段并发出。
            // 请注意，如果 slice 为空，我们仍然要迭代一次以发出单个零长度记录
            Status s;
            bool begin = true;
            do {
                // 获取剩余空间
                const int leftover = kBlockSize - block_offset_;

                assert(leftover >= 0); // 表示块中还有空间
                if (leftover < kHeaderSize) { // 如果当前块的大小小于块头所需要的空间
                    // leftover 大于0，表示当前块中还有空间，但不到 7 字节
                    if (leftover > 0) {
                        // 如果当前 block 剩下的空间已经不足 7 字节
                        static_assert(kHeaderSize == 7, "");
                        // 通过 x00 填完剩余空间
                        dest_->Append(Slice("\x00\x00\x00\x00\x00\x00", leftover));
                    }
                    block_offset_ = 0;
                }

                // 确保块中的数据全被使用上
                assert(kBlockSize - block_offset_ - kHeaderSize >= 0);

                // 除去已经使用的和头占用的空间后，剩余的空间
                const size_t avail = kBlockSize - block_offset_ - kHeaderSize;
                // 碎片的长度，如果长度超出块的大小，那大小以块的大小为准。
                // 如果碎片长度小于快的大小，那大小以碎片的大小为准。
                const size_t fragment_length = (left < avail) ? left : avail;

                RecordType type;
                // end = true, 表示碎片可以完整的放入当前块中
                //     = false, 表示碎片的大小超过了块的大小，具体怎么放方式有很多种
                const bool end = (left == fragment_length);
                if (begin && end) {
                    /** 当前记录的开始和结束都在当前 block，则为 kFullType */
                    type = kFullType;
                } else if (begin) {
                    /** 当前记录的开始字段在当前 block，则为kFirstType */
                    type = kFirstType;
                } else if (end) {
                    /** 当前记录的结束位置在当前 block，则为kLastType */
                    type = kLastType;
                } else {
                    /** 当前记录的开始和结束都不在当前 block，则为kMiddleType */
                    type = kMiddleType;
                }

                s = EmitPhysicalRecord(type, ptr, fragment_length);
                ptr += fragment_length; // 表示
                left -= fragment_length;
                begin = false;
            } while (s.ok() && left > 0);
            return s;
        }

        /**
         * 向 Block 中插入 Record 数据
         * @param t
         * @param ptr
         * @param length
         * @return
         */
        Status Writer::EmitPhysicalRecord(RecordType t, const char *ptr, size_t length) {
            // 必须容纳两个字节
            assert(length <= 0xffff);
            // 这里是限制，确保数据永远不会超过一个 block 的大小
            assert(block_offset_ + kHeaderSize + length <= kBlockSize);

            // LevelDB 是一种小端写磁盘的情况
            // LevelDB 使用的是小端字节序存储，低位字节排放在内存的低地址端
            // buf前面那个int是用来存放crc32的。
            char buf[kHeaderSize];
            // 写入长度: 这里先写入低8位
            buf[4] = static_cast<char>(length & 0xff);
            // 再写入高8位
            buf[5] = static_cast<char>(length >> 8);
            // 再写入类型
            buf[6] = static_cast<char>(t);

            // 计算记录类型和有效负载的 crc
            uint32_t crc = crc32c::Extend(type_crc_[t], ptr, length);
            crc = crc32c::Mask(crc); // 调成存储空间
            EncodeFixed32(buf, crc);

            // 写入 Header
            Status s = dest_->Append(Slice(buf, kHeaderSize));
            if (s.ok()) {
                // 写入 内容
                s = dest_->Append(Slice(ptr, length));
                // 当写完一个record之后，这里就立马 flush，但是有可能这个slice并不是完整的。
                if (s.ok()) {
                    s = dest_->Flush();
                }
            }
            // 在一个 block 里面的写入位置往前移。
            block_offset_ += kHeaderSize + length;
            return s;
        }

    }  // namespace log
}  // namespace leveldb
