//
// Created by torment on 2020/4/23.
//

#include "test/request/include/request.h"
#include <utility>

namespace crequest {

    request *request::get(std::string url) {
        return create_request(std::move(url), "GET");
    }

    request *request::post(std::string url) {
        return create_request(std::move(url), "POST");
    }

    task *request::get_task() {
        return nullptr;
    }

    request *request::create_request(std::string url, std::string request_method) {
        auto r = new request();
        r->set_url(std::move(url));
        r->set_method(std::move(request_method));
        return r;
    }

    void request::set_url(std::string u) {
        url = std::move(u);
    }

    void request::set_method(std::string m) {
        method = std::move(m);
    }

    request *request::set_tag(std::string t) {
        return nullptr;
    }
}