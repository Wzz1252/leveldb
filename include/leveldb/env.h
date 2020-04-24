// Copyright (c) 2011 The LevelDB Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.
//
// An Env is an interface used by the leveldb implementation to access
// operating system functionality like the filesystem etc.  Callers
// may wish to provide a custom Env object when opening a database to
// get fine gain control; e.g., to rate limit file system operations.
//
// All Env implementations are safe for concurrent access from
// multiple threads without any external synchronization.

#ifndef STORAGE_LEVELDB_INCLUDE_ENV_H_
#define STORAGE_LEVELDB_INCLUDE_ENV_H_

#include <stdarg.h>
#include <stdint.h>

#include <string>
#include <vector>

#include "leveldb/export.h"
#include "leveldb/status.h"

// This workaround can be removed when leveldb::Env::DeleteFile is removed.
#if defined(_WIN32)
// On Windows, the method name DeleteFile (below) introduces the risk of
// triggering undefined behavior by exposing the compiler to different
// declarations of the Env class in different translation units.
//
// This is because <windows.h>, a fairly popular header file for Windows
// applications, defines a DeleteFile macro. So, files that include the Windows
// header before this header will contain an altered Env declaration.
//
// This workaround ensures that the compiler sees the same Env declaration,
// independently of whether <windows.h> was included.
#if defined(DeleteFile)
#undef DeleteFile
#define LEVELDB_DELETEFILE_UNDEFINED
#endif  // defined(DeleteFile)
#endif  // defined(_WIN32)

namespace leveldb {

    class FileLock;

    class Logger;

    class RandomAccessFile;

    class SequentialFile;

    class Slice;

    class WritableFile;

    /**
     * 系统级别的操作，可以实现自己的逻辑
     */
    class LEVELDB_EXPORT Env {
    public:
        Env();

        Env(const Env &) = delete;

        Env &operator=(const Env &) = delete;

        virtual ~Env();

        // Return a default environment suitable for the current operating
        // system.  Sophisticated users may wish to provide their own Env
        // implementation instead of relying on this default environment.
        //
        // The result of Default() belongs to leveldb and must never be deleted.
        static Env *Default();

        // 创建一个对象，该对象顺序读取具有指定名称的文件。
        // 成功后，将指向新文件的指针存储在 *result 中，并返回 OK。
        // 失败后，将 nullptr 存储在 *result 中，并返回 非OK。如果该文件不存在，则返回非OK状态。文件不存在时，实现应返回 NotFound 状态。
        // 一次只能由一个线程访问返回的文件
        virtual Status NewSequentialFile(const std::string &fname, SequentialFile **result) = 0;

        // Create an object supporting random-access reads from the file with the
        // specified name.  On success, stores a pointer to the new file in
        // *result and returns OK.  On failure stores nullptr in *result and
        // returns non-OK.  If the file does not exist, returns a non-OK
        // status.  Implementations should return a NotFound status when the file does
        // not exist.
        //
        // The returned file may be concurrently accessed by multiple threads.
        virtual Status NewRandomAccessFile(const std::string &fname, RandomAccessFile **result) = 0;

        /**
         * 创建一个对象，该对象以指定的名称写入新文件。删除任何具有相同名称的现有文件并创建一个新文件。成功后，将指向新文件的指针存
         * 储在 *result 中，并返回 OK。失败时，将 nullptr 存储在 *result 中，并返回非 OK。
         *
         * 一次只能由一个线程访问返回的文件。
         *
         * @param fname
         * @param result
         * @return
         */
        virtual Status NewWritableFile(const std::string &fname, WritableFile **result) = 0;

        // Create an object that either appends to an existing file, or
        // writes to a new file (if the file does not exist to begin with).
        // On success, stores a pointer to the new file in *result and
        // returns OK.  On failure stores nullptr in *result and returns
        // non-OK.
        //
        // The returned file will only be accessed by one thread at a time.
        //
        // May return an IsNotSupportedError error if this Env does
        // not allow appending to an existing file.  Users of Env (including
        // the leveldb implementation) must be prepared to deal with
        // an Env that does not support appending.
        virtual Status NewAppendableFile(const std::string &fname,
                                         WritableFile **result);

        /** 如果命名文件存在，则返回true */
        virtual bool FileExists(const std::string &fname) = 0;

        /**
         * 在 *result 中存储指定目录的子级名称。
         *
         * 名称是相对于 "dir" 的。
         *
         * *results 的原始内容已删除。
         *
         * @param dir
         * @param result
         * @return
         */
        virtual Status GetChildren(const std::string &dir, std::vector<std::string> *result) = 0;

        // Delete the named file.
        //
        // The default implementation calls DeleteFile, to support legacy Env
        // implementations. Updated Env implementations must override RemoveFile and
        // ignore the existence of DeleteFile. Updated code calling into the Env API
        // must call RemoveFile instead of DeleteFile.
        //
        // A future release will remove DeleteDir and the default implementation of
        // RemoveDir.
        virtual Status RemoveFile(const std::string &fname);

        // DEPRECATED: Modern Env implementations should override RemoveFile instead.
        //
        // The default implementation calls RemoveFile, to support legacy Env user
        // code that calls this method on modern Env implementations. Modern Env user
        // code should call RemoveFile.
        //
        // A future release will remove this method.
        virtual Status DeleteFile(const std::string &fname);

        /** 创建指定的目录 */
        virtual Status CreateDir(const std::string &dirname) = 0;

        // Delete the specified directory.
        //
        // The default implementation calls DeleteDir, to support legacy Env
        // implementations. Updated Env implementations must override RemoveDir and
        // ignore the existence of DeleteDir. Modern code calling into the Env API
        // must call RemoveDir instead of DeleteDir.
        //
        // A future release will remove DeleteDir and the default implementation of
        // RemoveDir.
        virtual Status RemoveDir(const std::string &dirname);

        // DEPRECATED: Modern Env implementations should override RemoveDir instead.
        //
        // The default implementation calls RemoveDir, to support legacy Env user
        // code that calls this method on modern Env implementations. Modern Env user
        // code should call RemoveDir.
        //
        // A future release will remove this method.
        virtual Status DeleteDir(const std::string &dirname);

        // Store the size of fname in *file_size.
        virtual Status GetFileSize(const std::string &fname, uint64_t *file_size) = 0;

        // Rename file src to target.
        virtual Status RenameFile(const std::string &src, const std::string &target) = 0;

        /**
         * 锁定指定的文件。用于防止多个进程同时访问同一数据库。失败时，将 nullptr 存储在 *lock 中，并返回 OK。
         *
         * 成功后，将指向表示获取的锁的对象的指针存储在 *lock 中，并返回 OK。调用者应调用 UnlockFile(*lock) 解除锁定，如果该过程退
         * 出，则锁将自动释放。
         *
         * 如果其他人已经持有该锁，则立即以失败告终。也就是说，此调用不会等待现有的锁消失。
         *
         * 如果命名文件不存在，则可以创建该文件。
         *
         * @param fname
         * @param lock
         * @return
         */
        virtual Status LockFile(const std::string &fname, FileLock **lock) = 0;

        // Release the lock acquired by a previous successful call to LockFile.
        // REQUIRES: lock was returned by a successful LockFile() call
        // REQUIRES: lock has not already been unlocked.
        virtual Status UnlockFile(FileLock *lock) = 0;

        // 安排在后台线程中运行一次 "(*function)(arg)"
        //
        // "function" 可能在未指定的线程中运行。添加到同一 Env 的多个功能可以在不同的线程中同时运行。
        // 调用者可能不会假定后台工作项已序列化。
        virtual void Schedule(void (*function)(void *arg), void *arg) = 0;

        // Start a new thread, invoking "function(arg)" within the new thread.
        // When "function(arg)" returns, the thread will be destroyed.
        virtual void StartThread(void (*function)(void *arg), void *arg) = 0;

        // *path is set to a temporary directory that can be used for testing. It may
        // or may not have just been created. The directory may or may not differ
        // between runs of the same process, but subsequent calls will return the
        // same directory.
        virtual Status GetTestDirectory(std::string *path) = 0;

        // 创建并返回用于存储参考消息的日志文件
        virtual Status NewLogger(const std::string &fname, Logger **result) = 0;

        // Returns the number of micro-seconds since some fixed point in time. Only
        // useful for computing deltas of time.
        virtual uint64_t NowMicros() = 0;

        // Sleep/delay the thread for the prescribed number of micro-seconds.
        virtual void SleepForMicroseconds(int micros) = 0;
    };

    // 用于顺序读取文件的文件抽象类
    class LEVELDB_EXPORT SequentialFile {
    public:
        SequentialFile() = default;

        SequentialFile(const SequentialFile &) = delete;

        SequentialFile &operator=(const SequentialFile &) = delete;

        virtual ~SequentialFile();

        // Read up to "n" bytes from the file.  "scratch[0..n-1]" may be
        // written by this routine.  Sets "*result" to the data that was
        // read (including if fewer than "n" bytes were successfully read).
        // May set "*result" to point at data in "scratch[0..n-1]", so
        // "scratch[0..n-1]" must be live when "*result" is used.
        // If an error was encountered, returns a non-OK status.
        //
        // REQUIRES: External synchronization
        virtual Status Read(size_t n, Slice *result, char *scratch) = 0;

        // Skip "n" bytes from the file. This is guaranteed to be no
        // slower that reading the same data, but may be faster.
        //
        // If end of file is reached, skipping will stop at the end of the
        // file, and Skip will return OK.
        //
        // REQUIRES: External synchronization
        virtual Status Skip(uint64_t n) = 0;
    };

// A file abstraction for randomly reading the contents of a file.
    class LEVELDB_EXPORT RandomAccessFile {
    public:
        RandomAccessFile() = default;

        RandomAccessFile(const RandomAccessFile &) = delete;

        RandomAccessFile &operator=(const RandomAccessFile &) = delete;

        virtual ~RandomAccessFile();

        // Read up to "n" bytes from the file starting at "offset".
        // "scratch[0..n-1]" may be written by this routine.  Sets "*result"
        // to the data that was read (including if fewer than "n" bytes were
        // successfully read).  May set "*result" to point at data in
        // "scratch[0..n-1]", so "scratch[0..n-1]" must be live when
        // "*result" is used.  If an error was encountered, returns a non-OK
        // status.
        //
        // Safe for concurrent use by multiple threads.
        virtual Status Read(uint64_t offset, size_t n, Slice *result, char *scratch) const = 0;
    };

    /** 用于顺序写入的文件抽象。该实现必须提供缓冲，因为调用者可能一次将小片段附加到文件中。 */
    class LEVELDB_EXPORT WritableFile {
    public:
        WritableFile() = default;

        WritableFile(const WritableFile &) = delete;

        WritableFile &operator=(const WritableFile &) = delete;

        virtual ~WritableFile();

        virtual Status Append(const Slice &data) = 0;

        virtual Status Close() = 0;

        virtual Status Flush() = 0;

        virtual Status Sync() = 0;
    };

    // 编写日志消息的接口
    class LEVELDB_EXPORT Logger {
    public:
        Logger() = default;

        Logger(const Logger &) = delete;

        Logger &operator=(const Logger &) = delete;

        virtual ~Logger();

        // 以指定格式将条目写入日志文件
        virtual void Logv(const char *format, va_list ap) = 0;
    };

    /** 标识锁定的文件 */
    class LEVELDB_EXPORT FileLock {
    public:
        FileLock() = default;

        FileLock(const FileLock &) = delete;

        FileLock &operator=(const FileLock &) = delete;

        virtual ~FileLock();
    };

// Log the specified data to *info_log if info_log is non-null.
    void Log(Logger *info_log, const char *format, ...)
#if defined(__GNUC__) || defined(__clang__)
    __attribute__((__format__(__printf__, 2, 3)))
#endif
    ;

// A utility routine: write "data" to the named file.
    LEVELDB_EXPORT Status WriteStringToFile(Env *env, const Slice &data, const std::string &fname);

// A utility routine: read contents of named file into *data
    LEVELDB_EXPORT Status ReadFileToString(Env *env, const std::string &fname, std::string *data);

// An implementation of Env that forwards all calls to another Env.
// May be useful to clients who wish to override just part of the
// functionality of another Env.
    class LEVELDB_EXPORT EnvWrapper : public Env {
    public:
        // Initialize an EnvWrapper that delegates all calls to *t.
        explicit EnvWrapper(Env *t) : target_(t) {}

        virtual ~EnvWrapper();

        // Return the target to which this Env forwards all calls.
        Env *target() const { return target_; }

        // The following text is boilerplate that forwards all methods to target().
        Status NewSequentialFile(const std::string &f, SequentialFile **r) override {
            return target_->NewSequentialFile(f, r);
        }

        Status NewRandomAccessFile(const std::string &f, RandomAccessFile **r) override {
            return target_->NewRandomAccessFile(f, r);
        }

        Status NewWritableFile(const std::string &f, WritableFile **r) override {
            return target_->NewWritableFile(f, r);
        }

        Status NewAppendableFile(const std::string &f, WritableFile **r) override {
            return target_->NewAppendableFile(f, r);
        }

        bool FileExists(const std::string &f) override {
            return target_->FileExists(f);
        }

        Status GetChildren(const std::string &dir, std::vector<std::string> *r) override {
            return target_->GetChildren(dir, r);
        }

        Status RemoveFile(const std::string &f) override {
            return target_->RemoveFile(f);
        }

        Status CreateDir(const std::string &d) override {
            return target_->CreateDir(d);
        }

        Status RemoveDir(const std::string &d) override {
            return target_->RemoveDir(d);
        }

        Status GetFileSize(const std::string &f, uint64_t *s) override {
            return target_->GetFileSize(f, s);
        }

        Status RenameFile(const std::string &s, const std::string &t) override {
            return target_->RenameFile(s, t);
        }

        Status LockFile(const std::string &f, FileLock **l) override {
            return target_->LockFile(f, l);
        }

        Status UnlockFile(FileLock *l) override { return target_->UnlockFile(l); }

        void Schedule(void (*f)(void *), void *a) override {
            return target_->Schedule(f, a);
        }

        void StartThread(void (*f)(void *), void *a) override {
            return target_->StartThread(f, a);
        }

        Status GetTestDirectory(std::string *path) override {
            return target_->GetTestDirectory(path);
        }

        Status NewLogger(const std::string &fname, Logger **result) override {
            return target_->NewLogger(fname, result);
        }

        uint64_t NowMicros() override { return target_->NowMicros(); }

        void SleepForMicroseconds(int micros) override {
            target_->SleepForMicroseconds(micros);
        }

    private:
        Env *target_;
    };

}  // namespace leveldb

// This workaround can be removed when leveldb::Env::DeleteFile is removed.
// Redefine DeleteFile if it was undefined earlier.
#if defined(_WIN32) && defined(LEVELDB_DELETEFILE_UNDEFINED)
#if defined(UNICODE)
#define DeleteFile DeleteFileW
#else
#define DeleteFile DeleteFileA
#endif  // defined(UNICODE)
#endif  // defined(_WIN32) && defined(LEVELDB_DELETEFILE_UNDEFINED)

#endif  // STORAGE_LEVELDB_INCLUDE_ENV_H_
