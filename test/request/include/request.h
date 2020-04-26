//
// Created by torment on 2020/4/23.
//

#ifndef LEVELDB_REQUEST_H
#define LEVELDB_REQUEST_H

#include <iostream>
#include <functional>
#include "task.h"

/**
 * 使用方式：
 *
 * - 全局初始化
 * RequestConfig.init(true);
 */
namespace crequest {

    class task;

    // 每个 request 的具体操作
    // - get
    // - post
    // - put
    // - delete
    // - head
    // - options
    // - patch
    class request {
    public:
        request() = default;

        ~request() = default;

        static request *get(std::string url);

        static request *post(std::string url);

        static request *create_request(std::string url, std::string request_method);

        task *get_task();

        void set_url(std::string url);

        void set_method(std::string m);

        request *set_tag(std::string t);

    private :
        std::string url;
        std::string method;
        std::string tag;
    };
}

#endif //LEVELDB_REQUEST_H
