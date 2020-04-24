// Copyright (c) 2011 The LevelDB Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#ifndef STORAGE_LEVELDB_DB_SKIPLIST_H_
#define STORAGE_LEVELDB_DB_SKIPLIST_H_

/**
 * 线程安全
 *
 * 写操作需要外部同步，很可能是互斥锁。<br/>
 * 读取需要保证在读取过程中不会破坏 SkipList。除此之外，读取进度无需任何内部锁定或同步。
 *
 * (1) 在销毁 SkipList 之前，绝不会删除已分配的节点。由于我们从不删除任何跳过列表节点，因此代码可以保证这一点。<br/>
 * (2) 将节点链接到 SkipList 之后，除 next/prev 指针之外的 Node 的内容是不变的。只有 Insert() 会修改列表，并且谨慎初始化节点并使用
 *     发布存储将节点发布到一个或多个列表中。
 */

#include <atomic>
#include <cassert>
#include <cstdlib>

#include "util/arena.h"
#include "util/random.h"

namespace leveldb {

    class Arena;

    template<typename Key, class Comparator>
    class SkipList {
    private:
        struct Node;

    public:
        /**
         * 创建一个新的 SkipList 对象，该对象将使用 "cmp" 比较 key，并使用 "*arena" 分配内存。在舞台上分配的对象必须在跳
         * 过列表对象的生存期内保持分配状态。
         */
        explicit SkipList(Comparator cmp, Arena *arena);

        SkipList(const SkipList &) = delete;

        SkipList &operator=(const SkipList &) = delete;

        // 将 Key 插入 List 中
        // 要求：Key 不能相同。
        void Insert(const Key &key);

        // 如果列表中与键比较相等的条目，则返回 true
        bool Contains(const Key &key) const;

        // 对 跳表 的内容进行迭代
        class Iterator {
        public:
            // 在指定列表上初始化迭代器。
            // 返回的迭代器无效。
            explicit Iterator(const SkipList *list);

            // 如果迭代器位于有效节点上，则返回true。
            bool Valid() const;

            // 返回当前位置的键。
            // REQUIRES: Valid()
            const Key &key() const;

            // 前进到下一个位置
            // REQUIRES: Valid()
            void Next();

            // 前进到上一个位置
            // REQUIRES: Valid()
            void Prev();

            // Advance to the first entry with a key >= target
            void Seek(const Key &target);

            // Position at the first entry in list.
            // Final state of iterator is Valid() iff list is not empty.
            void SeekToFirst();

            // Position at the last entry in list.
            // Final state of iterator is Valid() iff list is not empty.
            void SeekToLast();

        private:
            const SkipList *list_;
            Node *node_;
            // Intentionally copyable
        };

    private:
        enum {
            kMaxHeight = 12
        };

        inline int GetMaxHeight() const {
            // memory_order_relaxed: 不对执行顺序做保证
            return max_height_.load(std::memory_order_relaxed);
        }

        Node *NewNode(const Key &key, int height);

        int RandomHeight();

        bool Equal(const Key &a, const Key &b) const { return (compare_(a, b) == 0); }

        // Return true if key is greater than the data stored in "n"
        bool KeyIsAfterNode(const Key &key, Node *n) const;

        // Return the earliest node that comes at or after key.
        // Return nullptr if there is no such node.
        //
        // If prev is non-null, fills prev[level] with pointer to previous
        // node at "level" for every level in [0..max_height_-1].
        Node *FindGreaterOrEqual(const Key &key, Node **prev) const;

        // Return the latest node with a key < key.
        // Return head_ if there is no such node.
        Node *FindLessThan(const Key &key) const;

        // Return the last node in the list.
        // Return head_ if list is empty.
        Node *FindLast() const;

        // Immutable after construction
        Comparator const compare_;
        Arena *const arena_;  // Arena used for allocations of nodes

        Node *const head_;

        // Modified only by Insert().  Read racily by readers, but stale
        // values are ok.
        std::atomic<int> max_height_;  // Height of the entire list

        // Read/written only by Insert().
        Random rnd_;
    };

    // Implementation details follow
    template<typename Key, class Comparator>
    struct SkipList<Key, Comparator>::Node {
        explicit Node(const Key &k) : key(k) {}

        Key const key;

        // Accessors/mutators for links.  Wrapped in methods so we can
        // add the appropriate barriers as necessary.
        Node *Next(int n) {
            assert(n >= 0);
            // Use an 'acquire load' so that we observe a fully initialized
            // version of the returned Node.
            return next_[n].load(std::memory_order_acquire);
        }

        void SetNext(int n, Node *x) {
            assert(n >= 0);
            // Use a 'release store' so that anybody who reads through this
            // pointer observes a fully initialized version of the inserted node.
            next_[n].store(x, std::memory_order_release);
        }

        /** 可以在几个位置安全使用的无障碍版本 */
        Node *NoBarrier_Next(int n) {
            assert(n >= 0);
            return next_[n].load(std::memory_order_relaxed);
        }

        /** 没有设置障碍 */
        void NoBarrier_SetNext(int n, Node *x) {
            assert(n >= 0);
            next_[n].store(x, std::memory_order_relaxed);
        }

    private:
        // 长度等于节点高度的数组。 next_ [0] 是最低级别的链接。
        std::atomic<Node *> next_[1];
    };

    template<typename Key, class Comparator>
    typename SkipList<Key, Comparator>::Node *SkipList<Key, Comparator>::NewNode(const Key &key, int height) {
        char *const node_memory = arena_->AllocateAligned(sizeof(Node) + sizeof(std::atomic<Node *>) * (height - 1));
        return new(node_memory) Node(key);
    }

    template<typename Key, class Comparator>
    inline SkipList<Key, Comparator>::Iterator::Iterator(const SkipList *list) {
        list_ = list;
        node_ = nullptr;
    }

    template<typename Key, class Comparator>
    inline bool SkipList<Key, Comparator>::Iterator::Valid() const {
        return node_ != nullptr;
    }

    template<typename Key, class Comparator>
    inline const Key &SkipList<Key, Comparator>::Iterator::key() const {
        assert(Valid());
        return node_->key;
    }

    template<typename Key, class Comparator>
    inline void SkipList<Key, Comparator>::Iterator::Next() {
        assert(Valid());
        node_ = node_->Next(0);
    }

    template<typename Key, class Comparator>
    inline void SkipList<Key, Comparator>::Iterator::Prev() {
        // Instead of using explicit "prev" links, we just search for the
        // last node that falls before key.
        assert(Valid());
        node_ = list_->FindLessThan(node_->key);
        if (node_ == list_->head_) {
            node_ = nullptr;
        }
    }

    template<typename Key, class Comparator>
    inline void SkipList<Key, Comparator>::Iterator::Seek(const Key &target) {
        node_ = list_->FindGreaterOrEqual(target, nullptr);
    }

    template<typename Key, class Comparator>
    inline void SkipList<Key, Comparator>::Iterator::SeekToFirst() {
        node_ = list_->head_->Next(0);
    }

    template<typename Key, class Comparator>
    inline void SkipList<Key, Comparator>::Iterator::SeekToLast() {
        node_ = list_->FindLast();
        if (node_ == list_->head_) {
            node_ = nullptr;
        }
    }

    template<typename Key, class Comparator>
    int SkipList<Key, Comparator>::RandomHeight() {
        // Increase height with probability 1 in kBranching
        static const unsigned int kBranching = 4;
        int height = 1;
        while (height < kMaxHeight && ((rnd_.Next() % kBranching) == 0)) {
            height++;
        }
        assert(height > 0);
        assert(height <= kMaxHeight);
        return height;
    }

    template<typename Key, class Comparator>
    bool SkipList<Key, Comparator>::KeyIsAfterNode(const Key &key, Node *n) const {
        // null n is considered infinite
        return (n != nullptr) && (compare_(n->key, key) < 0);
    }

    /**
     * 寻找更大或者等于的 Key
     * @tparam Key
     * @tparam Comparator
     * @param key
     * @param prev
     * @return
     */
    template<typename Key, class Comparator>
    typename SkipList<Key, Comparator>::Node *
    SkipList<Key, Comparator>::FindGreaterOrEqual(const Key &key, Node **prev) const {
        Node *x = head_;
        int level = GetMaxHeight() - 1;
        while (true) {
            Node *next = x->Next(level);
            if (KeyIsAfterNode(key, next)) {
                // 继续在这个 list 中搜索
                x = next;
            } else {
                if (prev != nullptr) prev[level] = x;
                if (level == 0) {
                    return next;
                } else {
                    // Switch to next list
                    level--;
                }
            }
        }
    }

    template<typename Key, class Comparator>
    typename SkipList<Key, Comparator>::Node *
    SkipList<Key, Comparator>::FindLessThan(const Key &key) const {
        Node *x = head_;
        int level = GetMaxHeight() - 1;
        while (true) {
            assert(x == head_ || compare_(x->key, key) < 0);
            Node *next = x->Next(level);
            if (next == nullptr || compare_(next->key, key) >= 0) {
                if (level == 0) {
                    return x;
                } else {
                    // Switch to next list
                    level--;
                }
            } else {
                x = next;
            }
        }
    }

    template<typename Key, class Comparator>
    typename SkipList<Key, Comparator>::Node *SkipList<Key, Comparator>::FindLast()
    const {
        Node *x = head_;
        int level = GetMaxHeight() - 1;
        while (true) {
            Node *next = x->Next(level);
            if (next == nullptr) {
                if (level == 0) {
                    return x;
                } else {
                    // Switch to next list
                    level--;
                }
            } else {
                x = next;
            }
        }
    }

    template<typename Key, class Comparator>
    SkipList<Key, Comparator>::SkipList(Comparator cmp, Arena *arena)
            : compare_(cmp),
              arena_(arena),
              head_(NewNode(0 /* any key will do */, kMaxHeight)),
              max_height_(1),
              rnd_(0xdeadbeef) {
        for (int i = 0; i < kMaxHeight; i++) {
            head_->SetNext(i, nullptr);
        }
    }

    /**
     * 插入一条数据
     * @tparam Key
     * @tparam Comparator
     * @param key
     */
    template<typename Key, class Comparator>
    void SkipList<Key, Comparator>::Insert(const Key &key) {
        // TODO(opt): We can use a barrier-free variant of FindGreaterOrEqual()
        // 这里是因为 Insert() 是外部同步的
        Node *prev[kMaxHeight];
        Node *x = FindGreaterOrEqual(key, prev);

        // 这个 SkipList 数据结构不允许重复插入
        assert(x == nullptr || !Equal(key, x->key));

        int height = RandomHeight();
        if (height > GetMaxHeight()) {
            for (int i = GetMaxHeight(); i < height; i++) {
                prev[i] = head_;
            }
            // It is ok to mutate max_height_ without any synchronization
            // with concurrent readers.  A concurrent reader that observes
            // the new value of max_height_ will see either the old value of
            // new level pointers from head_ (nullptr), or a new value set in
            // the loop below.  In the former case the reader will
            // immediately drop to the next level since nullptr sorts after all
            // keys.  In the latter case the reader will use the new node.
            max_height_.store(height, std::memory_order_relaxed);
        }

        x = NewNode(key, height);
        for (int i = 0; i < height; i++) {
            // NoBarrier_SetNext() suffices since we will add a barrier when we publish a pointer to "x" in prev[i].
            x->NoBarrier_SetNext(i, prev[i]->NoBarrier_Next(i));
            prev[i]->SetNext(i, x);
        }
    }

    template<typename Key, class Comparator>
    bool SkipList<Key, Comparator>::Contains(const Key &key) const {
        Node *x = FindGreaterOrEqual(key, nullptr);
        if (x != nullptr && Equal(key, x->key)) {
            return true;
        } else {
            return false;
        }
    }

}  // namespace leveldb

#endif  // STORAGE_LEVELDB_DB_SKIPLIST_H_
