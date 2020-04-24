//
// Created by torment on 2020/4/9.
//

#include "test/android/include/system.h"

namespace android {
    a_app* system::create_app() {
        application = new a_app();
        return application;
    }
}