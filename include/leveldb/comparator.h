// Copyright (c) 2011 The LevelDB Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#ifndef STORAGE_LEVELDB_INCLUDE_COMPARATOR_H_
#define STORAGE_LEVELDB_INCLUDE_COMPARATOR_H_

#include <string>

#include "leveldb/export.h"

namespace leveldb {

    class Slice;

    /**
     * Comparator 对象提供用作 sstable 或数据库中的键的切片的总顺序。
     *
     * Comparator 实现必须是线程安全的，因为 leveldb 可能会同时从多个线程调用其方法。
     */
    class LEVELDB_EXPORT Comparator {
    public:
        virtual ~Comparator();

        // Three-way comparison.  Returns value:
        //   < 0 iff "a" < "b",
        //   == 0 iff "a" == "b",
        //   > 0 iff "a" > "b"
        virtual int Compare(const Slice &a, const Slice &b) const = 0;

        /**
         * 比较器的名称。用于检查比较器是否不匹配（即，使用一个比较器访问由一个比较器创建的 DB。
         *
         * 只要比较器实现的更改会导致任何两个键的相对顺序更改，则此包的客户端应切换到新名称。
         *
         * 名称以 "leveldb" 开头。保留，并且此程序包的任何客户端均不得使用。
         * @return
         */
        virtual const char *Name() const = 0;

        // Advanced functions: these are used to reduce the space requirements
        // for internal data structures like index blocks.

        // If *start < limit, changes *start to a short string in [start,limit).
        // Simple comparator implementations may return with *start unchanged,
        // i.e., an implementation of this method that does nothing is correct.
        virtual void FindShortestSeparator(std::string *start,
                                           const Slice &limit) const = 0;

        // Changes *key to a short string >= *key.
        // Simple comparator implementations may return with *key unchanged,
        // i.e., an implementation of this method that does nothing is correct.
        virtual void FindShortSuccessor(std::string *key) const = 0;
    };

// Return a builtin comparator that uses lexicographic byte-wise
// ordering.  The result remains the property of this module and
// must not be deleted.
    LEVELDB_EXPORT const Comparator *BytewiseComparator();

}  // namespace leveldb

#endif  // STORAGE_LEVELDB_INCLUDE_COMPARATOR_H_
