//
// Created by torment on 2020/4/2.
//

#include "main_log.h"

void TLogger::log(const std::string &tag, const std::string &message) {
    std::cout << tag << " >> " << message << std::endl;
}