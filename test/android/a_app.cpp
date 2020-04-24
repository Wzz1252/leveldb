//
// Created by torment on 2020/4/9.
//

#include "test/android/include/a_app.h"
#include "test/android/include/activity.h"

namespace android {

    activity *a_app::create_activity() {
        auto *a = new activity();
        a->set_app(this);
        this->activities.push_back(a);
        return a;
    }

    void a_app::start_app() {
        if (activities.empty()) {
            std::cout << "没有 activity，创建一个" << std::endl;
            auto *a = new activity();
            a->set_flag(1);
            a->set_app(this);
            this->activities.push_back(a);
        }
        // 取出最上层的 activity，作为根布局
        activity *cur_act = activities[activities.size() - 1];

        cur_act->on_post_create();
    }
}