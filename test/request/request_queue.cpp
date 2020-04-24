//
// Created by torment on 2020/4/23.
//

#include <test/request/include/task_axios_impl.h>
#include "test/request/include/request_queue.h"

namespace crequest {

    request_queue *request_queue::create() {
        auto queue = new request_queue();
        return queue;
    }

    task_queue *request_queue::get_task_queue() {
        auto cursor = below_cursor == -1 ? 0 : below_cursor;
        return r_q[cursor];
    }

    request_queue *request_queue::add_request(class request *r) {
        auto queue = get_task_queue();
        if (!queue) {
            std::cout << "add_request task_queue 出现错误!" << std::endl;
            return this;
        }
        if (r->get_task()) {
            queue->request_task().push_back(r->get_task());
        } else {
            auto task = new task_axios_impl();
            task->set_request(r);
        }

        return nullptr;
    }

    void request_queue::request() {

    }
}