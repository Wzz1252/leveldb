//
// Created by torment on 2020/4/9.
//

#ifndef LEVELDB_ACTIVITY_H
#define LEVELDB_ACTIVITY_H

#include <iostream>
#include <test/android/view/include/root_view.h>
#include "context.h"

namespace android {

    class a_app;

    class activity : public context {
    public:
        activity() {};

        void set_app(a_app *a);

        void set_flag(int flag);

        int get_flag();

        void on_post_create();

    private :
        // 每个页面的唯一标识
        int flag = -1;
        // application
        a_app *app;
        // view
        root_view _root_v;

        void on_create();

        void on_destory();
    };

}

#endif //LEVELDB_ACTIVITY_H
