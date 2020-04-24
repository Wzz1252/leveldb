//
// Created by torment on 2020/4/9.
//

#ifndef LEVELDB_A_APP_H
#define LEVELDB_A_APP_H

#include <iostream>
#include <vector>

namespace android {

    class activity;

    /** Application */
    class a_app {
    public:
        a_app() = default;

        ~a_app() = default;

        activity *create_activity();

        void start_app();

        std::vector<activity *>activities = {};

        activity *create_activity(activity **pActivity);
    };
}

#endif //LEVELDB_A_APP_H
