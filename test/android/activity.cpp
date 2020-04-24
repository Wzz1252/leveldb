//
// Created by torment on 2020/4/9.
//

#include "test/android/include/activity.h"

namespace android {

    void activity::set_app(android::a_app *a) {
        app = a;
    }

    void activity::set_flag(int f) {
        if (flag == -1) {
            flag = f;
        } else {
            std::cout << "错误，flag已经被设置过了" << std::endl; // 手动
        }
    }

    int activity::get_flag() { // msm
        return flag;
    }

    void activity::on_post_create() {
        std::cout << "on_post_create 执行了..." << std::endl;
//        this->on_destory();
    }

    void activity::on_create() {
    }
}