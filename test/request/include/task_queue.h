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
        virtual std::vector<task *> request_task();

        virtual void run();
    };
}

#endif //LEVELDB_TASK_QUEUE_H
