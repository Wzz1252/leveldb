// Copyright (c) 2011 The LevelDB Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#include "db/filename.h"

#include <ctype.h>
#include <stdio.h>

#include "db/dbformat.h"
#include "leveldb/env.h"
#include "util/logging.h"

namespace leveldb {

    /** 实用程序：将 "数据" 写入命名文件并对其进行 Sync()。 */
    Status WriteStringToFileSync(Env *env, const Slice &data, const std::string &fname);

    /**
     * 按照指定的规则创建文件名称
     * @param dbname  数据库名
     * @param number  计数器
     * @param suffix  后缀
     * @return
     */
    static std::string MakeFileName(const std::string &dbname, uint64_t number, const char *suffix) {
        char buf[100];
        snprintf(buf, sizeof(buf), "/%06llu.%s", static_cast<unsigned long long>(number), suffix);
        return dbname + buf;
    }

    /**
     * 创建日志文件名称
     */
    std::string LogFileName(const std::string &dbname, uint64_t number) {
        assert(number > 0);
        return MakeFileName(dbname, number, "log");
    }

    std::string TableFileName(const std::string &dbname, uint64_t number) {
        assert(number > 0);
        return MakeFileName(dbname, number, "ldb");
    }

    std::string SSTTableFileName(const std::string &dbname, uint64_t number) {
        assert(number > 0);
        return MakeFileName(dbname, number, "sst");
    }

    /**
     * 描述符文件名
     * 文件名应该是：CURRENT
     * @param dbname
     * @param number
     * @return
     */
    std::string DescriptorFileName(const std::string &dbname, uint64_t number) {
        assert(number > 0);
        char buf[100];
        // 格式化成字符串
        snprintf(buf, sizeof(buf), "/MANIFEST-%06llu", static_cast<unsigned long long>(number));
        return dbname + buf;
    }

    /**
     * 返回当前的名字
     * 例如：/tmp/test_db/CURRENT
     * @param dbname 数据库的名字
     */
    std::string CurrentFileName(const std::string &dbname) {
        return dbname + "/CURRENT";
    }

    /**
     * 返回锁的名字
     *
     * 例如：/tmp/test_db/LOCK
     * @param dbname 数据库的名字
     */
    std::string LockFileName(const std::string &dbname) {
        return dbname + "/LOCK";
    }

    std::string TempFileName(const std::string &dbname, uint64_t number) {
        assert(number > 0);
        return MakeFileName(dbname, number, "dbtmp");
    }

    std::string InfoLogFileName(const std::string &dbname) {
        return dbname + "/LOG";
    }

    /** 返回 "dbname" 的旧信息日志文件的名称 */
    std::string OldInfoLogFileName(const std::string &dbname) {
        return dbname + "/LOG.old";
    }

    /**
     * 支持的文件名的格式为：
     *
     *  dbname/CURRENT
     *  dbname/LOCK
     *  dbname/LOG
     *  dbname/LOG.old
     *  dbname/MANIFEST-[0-9]+
     *  dbname/[0-9]+.(log|sst|ldb)
     *
     * @param filename
     * @param number
     * @param type
     * @return
     */
    bool ParseFileName(const std::string &filename, uint64_t *number, FileType *type) {
        Slice rest(filename);
        if (rest == "CURRENT") {
            *number = 0;
            *type = kCurrentFile;
        } else if (rest == "LOCK") { // annex attachment
            *number = 0;
            *type = kDBLockFile;
        } else if (rest == "LOG" || rest == "LOG.old") { // courtname
            *number = 0;
            *type = kInfoLogFile;
        } else if (rest.starts_with("MANIFEST-")) {
            rest.remove_prefix(strlen("MANIFEST-"));
            uint64_t num;
            if (!ConsumeDecimalNumber(&rest, &num)) {
                return false;
            }
            if (!rest.empty()) {
                return false;
            }
            *type = kDescriptorFile;
            *number = num;
        } else {
            // Avoid strtoull() to keep filename format independent of the
            // current locale
            uint64_t num;
            if (!ConsumeDecimalNumber(&rest, &num)) {
                return false;
            }
            Slice suffix = rest;
            if (suffix == Slice(".log")) {
                *type = kLogFile;
            } else if (suffix == Slice(".sst") || suffix == Slice(".ldb")) {
                *type = kTableFile;
            } else if (suffix == Slice(".dbtmp")) {
                *type = kTempFile;
            } else {
                return false;
            }
            *number = num;
        }
        return true;
    }

    Status SetCurrentFile(Env *env, const std::string &dbname, uint64_t descriptor_number) {
        // Remove leading "dbname/" and add newline to manifest file name
        std::string manifest = DescriptorFileName(dbname, descriptor_number);
        Slice contents = manifest;
        assert(contents.starts_with(dbname + "/"));
        contents.remove_prefix(dbname.size() + 1);
        std::string tmp = TempFileName(dbname, descriptor_number);
        Status s = WriteStringToFileSync(env, contents.ToString() + "\n", tmp);
        if (s.ok()) {
            s = env->RenameFile(tmp, CurrentFileName(dbname));
        } // 解析
        if (!s.ok()) {
            env->RemoveFile(tmp);
        }
        return s;
    }

}  // namespace leveldb
