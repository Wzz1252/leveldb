//
// Created by torment on 2020/4/26.
//

#ifndef LEVELDB_TTEMPLATE_H
#define LEVELDB_TTEMPLATE_H

#include <iostream>

namespace class_test {
    template<typename T, typename F>
    class ttemplate {
    public:
        ttemplate() = default;

        void print(T t, F f);
    };
}

#endif //LEVELDB_TTEMPLATE_H