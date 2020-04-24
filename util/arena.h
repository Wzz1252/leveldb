// Copyright (c) 2011 The LevelDB Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#ifndef STORAGE_LEVELDB_UTIL_ARENA_H_
#define STORAGE_LEVELDB_UTIL_ARENA_H_

#include <atomic>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <vector>

namespace leveldb {

    class Arena {
    public:
        Arena();

        Arena(const Arena &) = delete;

        Arena &operator=(const Arena &) = delete;

        ~Arena();

        // Return a pointer to a newly allocated memory block of "bytes" bytes.
        char *Allocate(size_t bytes);

        // Allocate memory with the normal alignment guarantees provided by malloc.
        char *AllocateAligned(size_t bytes);

        // Returns an estimate of the total memory usage of data allocated
        // by the arena.
        size_t MemoryUsage() const {
            return memory_usage_.load(std::memory_order_relaxed);
        }

    private:
        char *AllocateFallback(size_t bytes);

        char *AllocateNewBlock(size_t block_bytes);

        /** 分配状态 */
        char *alloc_ptr_;
        /** 分配剩余字节数 */
        size_t alloc_bytes_remaining_;

        /** new [] 个分配的内存块的数组 */
        std::vector<char *> blocks_;

        /**
         * arena 的总内存使用量
         *
         * TODO(costan): 该成员是通过原子访问的，而其他成员则没有任何锁定地访问。这个可以吗？
         */
        std::atomic<size_t> memory_usage_;
    };

    inline char *Arena::Allocate(size_t bytes) {
        // 如果我们允许分配 0 字节，则返回内容的语义有点混乱，因此我们在此处不允许使用它们（内部不需要它们）。
        assert(bytes > 0);

        if (bytes <= alloc_bytes_remaining_) {
            char *result = alloc_ptr_;
            alloc_ptr_ += bytes;
            alloc_bytes_remaining_ -= bytes;
            return result;
        }
        return AllocateFallback(bytes);
    }

}  // namespace leveldb

#endif  // STORAGE_LEVELDB_UTIL_ARENA_H_
