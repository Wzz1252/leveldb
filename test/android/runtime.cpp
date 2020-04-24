//
// Created by torment on 2020/4/9.
//

#include "test/android/include/runtime.h"

namespace android {
    /** 启动 runtime */
    void runtime::start() {
        system = new android::system();
        std::cout << "runtime start()" << std::endl;
    }

    a_app *runtime::create_application() {
        return system->create_app();
    }
}