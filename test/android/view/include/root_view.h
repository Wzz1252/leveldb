//
// Created by torment on 2020/4/9.
//

#ifndef LEVELDB_ROOT_VIEW_H
#define LEVELDB_ROOT_VIEW_H

#include "view.h"

namespace android {

    class root_view : public view {
    public:

    private:
        view status_view;   // 状态栏
        view toolbar_view;  // 工具栏
        view conent_view;   // 内容
        view bottom_view;   // 导航栏
    };
}

#endif //LEVELDB_ROOT_VIEW_H
