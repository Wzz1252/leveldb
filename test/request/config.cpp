//
// Created by torment on 2020/4/26.
//

#include "test/request/include/config.h"
#include "test/request/include/task.h"

namespace crequest {

    crequest::below_type config::get_below_type() {
        return below_type;
    }

    void config::set_task(crequest::task *t) {
        task = t;
    }

    task *config::get_task() {
        return task;
    }
}