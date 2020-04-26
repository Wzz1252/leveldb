//
// Created by torment on 2020/4/2.
//

#include <iostream>
#include <include/leveldb/db.h>

class TLogger {
public:
    static void log(const std::string &tag, const std::string &message);
};