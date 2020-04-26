//
// Created by torment on 2020/4/26.
//

#ifndef LEVELDB_TASK_QUEUE_SERIAL_H
#define LEVELDB_TASK_QUEUE_SERIAL_H

#include "task_queue.h"

namespace crequest {
    // 执行串行队列
    class task_queue_serial : public task_queue {
    public:
        void run() override;

        void set_task(std::vector<task *> *t);

        std::vector<task *> *get_request_task() override;

        void set_request_task(task *t) override;

    private:
        void next_request(std::vector<task *> *rt);
    };
}

#endif //LEVELDB_TASK_QUEUE_SERIAL_H
