//
// Created by torment on 2020/4/23.
//

#ifndef LEVELDB_TASK_H
#define LEVELDB_TASK_H

namespace crequest {

    class request;

    class task {
    public:
        virtual void run() = 0;

        virtual request *get_request() = 0;

        virtual void set_request(request *r) = 0;
    };
}


#endif //LEVELDB_TASK_H
