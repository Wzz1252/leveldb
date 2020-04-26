//
// Created by torment on 2020/4/23.
//

#ifndef LEVELDB_REQUEST_QUEUE_H
#define LEVELDB_REQUEST_QUEUE_H

#include <iostream>
#include <vector>
#include "request.h"
#include "task_queue.h"
#include "config.h"

// 默认磁盘缓存是 2MB
#define DEFAULT_DISK_USAGE_BYTES 2 * 1024 * 1024;

namespace crequest {

    /**
     * 使用方式：
     * - 上来先配置一下 request
     *   request_config.init(new config()); // 可写可不写，不写使用默认的配置文件
     * - 之后直接使用
     *   request_queue.create(xxx)...
     *
     * request_queue method:
     * - create
     * - set_config
     * - set_show_loading
     * - set_show_err_msg
     * - add_request
     * - request
     * - below_serial
     * - below_parallel
     * - set_success_listener
     * - set_fail_listener
     * - set_complete_listener
     *
     */
    class request_queue {
    public:
        request_queue() = default;

        ~request_queue() = default;

        // 创建一个 request_queue 队列
        static request_queue *create();

        // 创建一个 request_queue 队列，并接受一个 config 对象
        static request_queue *create(config *f);

        // 往队列中添加一个 request
        request_queue *add_request(request *r);

        // 添加配置文件
        // 配置文件规则：
        // 如果 request 有自己的配置文件，则读取自己的配置文件
        // 如果 request 没有自己的配置文件，则读取 request_queue 的配置文件
        // 如果 request_queue 没有自己的配置文件，在执行 create 函数时，会创建一个默认的配置文件
        request_queue *set_config(config *f);

        config *get_config();

        // 添加串行队列
        request_queue *below_serial();

        // 添加并行队列
        request_queue *below_parallel();

        void request(request *r);
        void request();

    private:
        // 请求队列的格式
        // request_queue
        //   task_queue
        //     task
        //     task
        //   task_queue
        //     task
        //     task
        std::vector<task_queue *> request_q;
        /**
         * 每执行一次 {@link below_serial()} 和 {@link below_parallel()} 计数器都会加一。
         * 计数器用来确定 request_queue_task 在队列中的位置
         */
        int below_cursor = -1;
        // 默认配置文件
        config *default_config;
        // 当前队列时候已经取消
        bool canceled = false;

        task_queue *get_task_queue();
    };

}

#endif //LEVELDB_REQUEST_QUEUE_H
