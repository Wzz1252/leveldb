//
// Created by torment on 2020/4/9.
//

#ifndef LEVELDB_SYSTEM_H
#define LEVELDB_SYSTEM_H

#include "a_app.h"

namespace android {

    class system {
    public:
        system() = default;

        ~system() = default;

        a_app* create_app();

    private :
        a_app *application;
    };
}

#endif //LEVELDB_SYSTEM_H
