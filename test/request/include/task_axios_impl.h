//
// Created by torment on 2020/4/23.
//

#ifndef LEVELDB_TASK_AXIOS_IMPL_H
#define LEVELDB_TASK_AXIOS_IMPL_H

#include "task.h"
#include "request.h"

namespace crequest {
    class task;

    class request;

    class task_axios_impl : public task {
    public:
        task_axios_impl() = default;

        void set_request(request *r) override;

        void run() override;

        request *get_request() override;

    private:
        request *r; //
    };
}

#endif //LEVELDB_TASK_AXIOS_IMPL_H
