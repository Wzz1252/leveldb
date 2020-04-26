//
// Created by torment on 2020/4/23.
//

#include <test/request/include/task_axios_impl.h>
#include <test/request/include/task_queue_serial.h>
#include <test/request/include/default_config.h>
#include "test/request/include/request_queue.h"

namespace crequest {

    request_queue *request_queue::create() {
        return create(nullptr);
    }

    request_queue *request_queue::create(config *c) {
        auto *queue = new request_queue();

        if (c == nullptr) {
            c = new crequest::default_config();
        }
        queue->set_config(c);

        auto below_type = queue->get_config()->get_below_type();
        if (below_type == BELOW_SERIAL) {
            queue->below_serial();
        } else if (below_type == BELOW_PARALLEL) {
            queue->below_parallel();
        } else {
            std::cout << "没有找到指定的队列模式" << std::endl;
        }

        return queue;
    }

    request_queue *request_queue::add_request(class request *r) {
        auto queue = get_task_queue();
        if (!queue) {
            std::cout << "add_request task_queue error!" << std::endl;
            return this;
        }

        if (r->get_task()) {
            queue->set_request_task(r->get_task());
        } else {
            task *task = default_config->get_task();
            std::cout << "create default task_axios_impl: " << task << std::endl;
            if (task == nullptr) {
                task = new task_axios_impl();
            }
            task->set_request(r);
            queue->set_request_task(task);
        }

        return this;
    }

    void request_queue::request(crequest::request *r) {
        if (r != nullptr) {
            add_request(r);
            request();
        } else {
            request();
        }
    }

    void request_queue::request() {
        for (auto &i : request_q) {
            std::cout << "request" << std::endl;
            i->run();
        }
    }

    request_queue *request_queue::set_config(config *f) {
        default_config = f;
        return this;
    }

    config *request_queue::get_config() {
        return default_config;
    }

    request_queue *request_queue::below_serial() {
        auto *tqs = new task_queue_serial();
        tqs->set_task(new std::vector<task *>());
        request_q.push_back(tqs);
        this->below_cursor++;
        return this;
    }

    request_queue *request_queue::below_parallel() {
        auto *tqs = new task_queue_serial();
        tqs->set_task(new std::vector<task *>());
        request_q.push_back(tqs);
        this->below_cursor++;
        return this;
    }

    task_queue *request_queue::get_task_queue() {
        // 获得最后一个任务队列
        auto cursor = below_cursor == -1 ? 0 : below_cursor;
        return request_q[cursor];
    }

}