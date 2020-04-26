//
// Created by torment on 2020/4/26.
//

#ifndef LEVELDB_DEFAULT_CONFIG_H
#define LEVELDB_DEFAULT_CONFIG_H

#include "config.h"
#include "task_axios_impl.h"

namespace crequest {
    class config;

    class task_axios_impl;

    class default_config : public config {
    public:
        default_config() = default;

    private:
        crequest::below_type below_type = BELOW_PARALLEL;
        crequest::task *task = new crequest::task_axios_impl();
    };
}

#endif //LEVELDB_DEFAULT_CONFIG_H
