//
// Created by torment on 2020/4/9.
//

#ifndef LEVELDB_RUNTIME_H
#define LEVELDB_RUNTIME_H

#include "system.h"
#include <iostream>

namespace android {

    class runtime {
    public:
        void start();

        a_app* create_application();

    private:
        system *system;
    };
}

#endif //LEVELDB_RUNTIME_H
