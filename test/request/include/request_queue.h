//
// Created by torment on 2020/4/23.
//

#ifndef LEVELDB_REQUEST_QUEUE_H
#define LEVELDB_REQUEST_QUEUE_H

#include <iostream>
#include <vector>
#include "request.h"
#include "task_queue.h"

namespace crequest {

    class request_queue {
    public:
        request_queue() = default;

        ~request_queue() = default;

        static request_queue *create();

        request_queue *add_request(request *r);

        void request();

    private:
        std::vector<task_queue *> r_q;
        /**
         * 每执行一次 {@link belowSerial()} 和 {@link belowParallel()} 计数器都会加一。
         * 计数器用来确定 RequestQueueTask 在队列中的位置
         */
        int below_cursor = -1;

        task_queue *get_task_queue();
    };

}

#endif //LEVELDB_REQUEST_QUEUE_H
