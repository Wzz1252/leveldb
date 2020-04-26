//
// Created by torment on 2020/4/23.
//

#ifndef LEVELDB_TASK_QUEUE_H
#define LEVELDB_TASK_QUEUE_H

#include <iostream>
#include <vector>
#include "task.h"

namespace crequest {
    class task_queue {
    public:
        virtual void run() = 0;

        virtual std::vector<task *> *get_request_task() = 0;

        virtual void set_request_task(task *t) = 0;

    protected:
        std::vector<task *> *request_task;
    };
}

#endif //LEVELDB_TASK_QUEUE_H
