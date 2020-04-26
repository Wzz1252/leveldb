//
// Created by torment on 2020/4/26.
//

#include <list>
#include "test/request/include/task_queue_serial.h"

namespace crequest {

    void task_queue_serial::run() {
        next_request(request_task);
    }

    void task_queue_serial::next_request(std::vector<task *> *rt) {
        if (rt->size() <= 0) {
            return;
        }
        task *task = rt->back();
        rt->pop_back();
        if (task == nullptr) {
            return;
        }

        request *request = task->get_request();
        // TODO 假设成功
        task->run();
        std::cout << "执行..." << std::endl;
        next_request(rt);
    }

    void task_queue_serial::set_task(std::vector<task *> *t) {
        request_task = t;
    }

    std::vector<task *> *task_queue_serial::get_request_task() {
        return request_task;
    }

    void task_queue_serial::set_request_task(task *t) {
        request_task->push_back(t);
    }

}