// Copyright (c) 2011 The LevelDB Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.
//
// Slice is a simple structure containing a pointer into some external
// storage and a size.  The user of a Slice must ensure that the slice
// is not used after the corresponding external storage has been
// deallocated.
//
// Multiple threads can invoke const methods on a Slice without
// external synchronization, but if any of the threads may call a
// non-const method, all threads accessing the same Slice must use
// external synchronization.

#ifndef STORAGE_LEVELDB_INCLUDE_SLICE_H_
#define STORAGE_LEVELDB_INCLUDE_SLICE_H_

#include <assert.h>
#include <stddef.h>
#include <string.h>

#include <string>

#include "leveldb/export.h"

namespace leveldb {

    /**
     * 切片
     * 对一段连续内存的引用
     */
    class LEVELDB_EXPORT Slice {
    public:
        /** 创建一个空切片 */
        Slice() : data_(""), size_(0) {}

        // 创建一个引用 d[0, n-1] 的切片。
        Slice(const char *d, size_t n) : data_(d), size_(n) {}

        // 创建引用 "s" 内容的切片
        Slice(const std::string &s) : data_(s.data()), size_(s.size()) {}

        // 创建一个指向 s[0, strlen(s) - 1] 的切片
        Slice(const char *s) : data_(s), size_(strlen(s)) {}

        /** 故意复制 */
        Slice(const Slice &) = default;

        Slice &operator=(const Slice &) = default;

        /** 返回一个指向引用数据开头的指针 */
        const char *data() const { return data_; }

        /** 返回引用数据的长度（以字节为单位） */
        size_t size() const { return size_; }

        /** 如果引用数据的长度为零，则返回 true */
        bool empty() const { return size_ == 0; }

        /**
         * 返回引用数据中的第i个字节。
         * 要求：n < size()
         */
        char operator[](size_t n) const {
            assert(n < size());
            return data_[n];
        }

        /** 更改此切片以引用一个空数组 */
        void clear() {
            data_ = "";
            size_ = 0;
        }

        /**
         * 从该片中删除前 "n" 个字节。
         * 将标记向后移动，表示 n 个字节已被使用
         */
        void remove_prefix(size_t n) {
            assert(n <= size());
            data_ += n;
            size_ -= n;
        }

        // Return a string that contains the copy of the referenced data.
        std::string ToString() const { return std::string(data_, size_); }

        // Three-way comparison.  Returns value:
        //   <  0 iff "*this" <  "b",
        //   == 0 iff "*this" == "b",
        //   >  0 iff "*this" >  "b"
        int compare(const Slice &b) const;

        // Return true iff "x" is a prefix of "*this"
        bool starts_with(const Slice &x) const {
            return ((size_ >= x.size_) && (memcmp(data_, x.data_, x.size_) == 0));
        }

    private:
        /** 切片 Header 中的内容，包括：checksum、length、type */
        const char *data_;
        /** 头的文件大小 */
        size_t size_;
    };

    inline bool operator==(const Slice &x, const Slice &y) {
        return ((x.size() == y.size()) &&
                (memcmp(x.data(), y.data(), x.size()) == 0));
    }

    inline bool operator!=(const Slice &x, const Slice &y) { return !(x == y); }

    inline int Slice::compare(const Slice &b) const {
        const size_t min_len = (size_ < b.size_) ? size_ : b.size_;
        int r = memcmp(data_, b.data_, min_len);
        if (r == 0) {
            if (size_ < b.size_)
                r = -1;
            else if (size_ > b.size_)
                r = +1;
        }
        return r;
    }

}  // namespace leveldb

#endif  // STORAGE_LEVELDB_INCLUDE_SLICE_H_
