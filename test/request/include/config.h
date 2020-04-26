//
// Created by torment on 2020/4/26.
//

#ifndef LEVELDB_CONFIG_H
#define LEVELDB_CONFIG_H

#include <test/request/enum/request_enum.h>

namespace crequest {

    class task;

    // 全局配置文件
    class config {
    public:
        crequest::below_type get_below_type();

        crequest::task *get_task();

        void set_task(crequest::task *t);

    private :
        crequest::below_type below_type = BELOW_PARALLEL;
        crequest::task *task;
    };
}


#endif //LEVELDB_CONFIG_H
