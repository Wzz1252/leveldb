//
// Created by torment on 2020/4/23.
//

#include "test/request/include/task_axios_impl.h"

namespace crequest {
    void task_axios_impl::set_request(request *rs) {
        r = rs;
    }

    void task_axios_impl::run() {
        std::function<void(int)> f = [](int a) { std::cout << "function" << std::endl; };
         set_success_listener<int>(f);
        std::cout << "task_axios_impl run..." << std::endl;
    }

    request *task_axios_impl::get_request() {
        return nullptr;
    }

    template<typename D>
    void task_axios_impl::set_success_listener(std::function<void(D)> &listener) {
    }

}