// Copyright (c) 2011 The LevelDB Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#ifndef STORAGE_LEVELDB_INCLUDE_OPTIONS_H_
#define STORAGE_LEVELDB_INCLUDE_OPTIONS_H_

#include <stddef.h>

#include "leveldb/export.h"

namespace leveldb {

    class Cache;

    class Comparator;

    class Env;

    class FilterPolicy;

    class Logger;

    class Snapshot;

// DB contents are stored in a set of blocks, each of which holds a
// sequence of key,value pairs.  Each block may be compressed before
// being stored in a file.  The following enum describes which
// compression method (if any) is used to compress a block.

    /**
     * leveldb 中数据库的内容是保存在一系列的 blocks 中的，每个 block 包含一系列 K-V 键值对。
     * 每个 block 的数据在保存前可能会被压缩，压缩方法枚举类如下。
     */
    enum CompressionType {
        // 注意：请勿更改现有条目的值，因为它们是磁盘上持久性格式的一部分。
        kNoCompression = 0x0,
        kSnappyCompression = 0x1
    };

    /** 用于控制数据库行为的选项 (传递给 DB::Open) */
    struct LEVELDB_EXPORT Options {
        // 使用所有字段的默认值创建一个 Options 对象
        Options();

        // -------------------
        // 影响行为的参数

        /**
         * 比较器，用于定义表中键的顺序。
         * 默认值：使用字典顺序逐字节排序的比较器
         *
         * 要求：客户端必须确保此处提供的比较器具有与相同 DB 上先前打开的调用所提供的比较器完全相同的名称和顺序键。
         */
        const Comparator *comparator;

        /** 如果是 true，db 目录不存在的时候创建。 */
        bool create_if_missing = false;

        /** 如果是 true，db目录已经存在时是否报错  */
        bool error_if_exists = false;

        /**
         * 如果为 true，则该实现将对正在处理的数据进行主动检查，并且在检测到任何错误时将尽早停止。
         *
         * 这可能会有无法预料的后果：例如，一个数据库条目的损坏可能导致大量条目变得不可读或整个数据库无法打开。
         */
        bool paranoid_checks = false;

        /**
         * 使用指定的对象与环境进行交互，例如读取/写入文件，安排后台工作等。
         * 默认: Env::Default()
         */
        Env *env;

        // 如果数据库生成的任何内部进度/错误信息为 非null，则将其写入 info_log；
        // 如果 info_log 为 null，则将其写入与 DB 内容存储在同一目录中的文件。
        Logger *info_log = nullptr;

        // -------------------
        // 影响性能的参数

        // 在转换为已排序的磁盘文件之前，要在内存中积累的数据量（由未排序的磁盘日志支持）。
        //
        // 较大的值可提高性能，尤其是在大负载期间。
        // 最多可以同时在内存中保留两个写缓冲区，因此您可能希望调整此参数以控制内存使用情况。同样，较大
        // 的写缓冲区将导致下次打开数据库时较长的恢复时间。
        size_t write_buffer_size = 4 * 1024 * 1024;

        // 数据库可以使用的打开文件数。如果数据库的工作集很大（每2MB的工作集预算一个打开的文件），则
        // 可能需要增加此数量。
        int max_open_files = 1000;

        // Control over blocks (user data is stored in a set of blocks, and
        // a block is the unit of reading from disk).

        // 如果非空，则将指定的缓存用于块。
        // 如果为null，leveldb 将自动创建并使用 8MB 内部缓存。
        Cache *block_cache = nullptr;

        // 每个 block 打包的用户数据的近似大小。注意，此处指定的块大小对应于未压缩的数据。如果启用了压缩，则
        // 从磁盘读取的单元的实际大小可能会更小。该参数可以动态更改。
        size_t block_size = 4 * 1024;

        // Number of keys between restart points for delta encoding of keys.
        // This parameter can be changed dynamically.  Most clients should
        // leave this parameter alone.
        int block_restart_interval = 16;

        // Leveldb 将在切换到新字节之前将最多字节的数量写入文件。
        //
        // 大多数客户端应保留此参数。但是，如果文件系统对较大的文件更有效，则可以考虑增加该值。不利的
        // 一面是压缩时间更长，从而导致延迟时间/性能下降。增加此参数的另一个原因可能是在最初填充大型数
        // 据库时。
        size_t max_file_size = 2 * 1024 * 1024;

        // Compress blocks using the specified compression algorithm.  This
        // parameter can be changed dynamically.
        //
        // Default: kSnappyCompression, which gives lightweight but fast
        // compression.
        //
        // Typical speeds of kSnappyCompression on an Intel(R) Core(TM)2 2.4GHz:
        //    ~200-500MB/s compression
        //    ~400-800MB/s decompression
        // Note that these speeds are significantly faster than most
        // persistent storage speeds, and therefore it is typically never
        // worth switching to kNoCompression.  Even if the input data is
        // incompressible, the kSnappyCompression implementation will
        // efficiently detect that and will switch to uncompressed mode.
        CompressionType compression = kSnappyCompression;

        // EXPERIMENTAL: If true, append to existing MANIFEST and log files
        // when a database is opened.  This can significantly speed up open.
        //
        // Default: currently false, but may become true later.
        bool reuse_logs = false;

        // If non-null, use the specified filter policy to reduce disk reads.
        // Many applications will benefit from passing the result of
        // NewBloomFilterPolicy() here.
        const FilterPolicy *filter_policy = nullptr;
    };

    // 控制读取操作的选项
    struct LEVELDB_EXPORT ReadOptions {
        ReadOptions() = default;

        // If true, all data read from underlying storage will be
        // verified against corresponding checksums.
        bool verify_checksums = false;

        // Should the data read for this iteration be cached in memory?
        // Callers may wish to set this field to false for bulk scans.
        bool fill_cache = true;

        // If "snapshot" is non-null, read as of the supplied snapshot
        // (which must belong to the DB that is being read and which must
        // not have been released).  If "snapshot" is null, use an implicit
        // snapshot of the state at the beginning of this read operation.
        const Snapshot *snapshot = nullptr;
    };

    // Options that control write operations
    struct LEVELDB_EXPORT WriteOptions {
        WriteOptions() = default;

        // If true, the write will be flushed from the operating system
        // buffer cache (by calling WritableFile::Sync()) before the write
        // is considered complete.  If this flag is true, writes will be
        // slower.
        //
        // If this flag is false, and the machine crashes, some recent
        // writes may be lost.  Note that if it is just the process that
        // crashes (i.e., the machine does not reboot), no writes will be
        // lost even if sync==false.
        //
        // In other words, a DB write with sync==false has similar
        // crash semantics as the "write()" system call.  A DB write
        // with sync==true has similar crash semantics to a "write()"
        // system call followed by "fsync()".
        bool sync = false;
    };

}  // namespace leveldb

#endif  // STORAGE_LEVELDB_INCLUDE_OPTIONS_H_
