//
// Created by torment on 2020/4/13.
//

#ifndef LEVELDB_CONTEXT_H
#define LEVELDB_CONTEXT_H

namespace android {

    class context { // 销毁
    public:
        void on_create();

        void on_resume();

        void on_stop();

        void on_destory();
    };
}


#endif //LEVELDB_CONTEXT_H
