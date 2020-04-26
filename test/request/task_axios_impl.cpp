//
// Created by torment on 2020/4/23.
//

#include "test/request/include/task_axios_impl.h"

namespace crequest {
    void task_axios_impl::set_request(request *rs) {
        r = rs;
    }

    void task_axios_impl::run() {

    }

    request *task_axios_impl::get_request() {
        return nullptr;
    }
}