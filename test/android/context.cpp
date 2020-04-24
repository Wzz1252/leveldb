//
// Created by torment on 2020/4/9.
//

#include "test/android/include/a_app.h"
#include "test/android/include/activity.h"

namespace android {
    void context::on_destory() {
        std::cout << "base context on_destory" << std::endl;
    }
}