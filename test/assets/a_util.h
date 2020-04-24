//
// Created by torment on 2020/4/7.
//

#ifndef LEVELDB_A_UTIL_H
#define LEVELDB_A_UTIL_H

namespace assets {

    /**
     * 常用工具类
     */
    class a_util {
    public:
        /**
         * 求幂
         * @param value 值
         * @param years 幂
         * @param result 将结果保存到对象中
         */
        static void exponentiation(double value, int power, double &result);
    };

}

#endif //LEVELDB_A_UTIL_H
