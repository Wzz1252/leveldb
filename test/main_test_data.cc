//
// Created by torment on 2020/4/2.
//

#include "main_test_data.h"
#include "main_log.h"

std::string test_array[52] = {
        "a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k",
        "l", "m", "n", "o", "p", "q", "r", "s", "t", "u", "v",
        "w", "x", "y", "z",
        "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K",
        "L", "M", "N", "O", "P", "Q", "R", "S", "T", "U", "V",
        "W", "X", "Y", "Z"
};

std::string TAG = "main_test_data";

/**
 * 生成随机数
 * @return
 */
std::string TestData::buildRandomTestData() {
    std::default_random_engine generator(time(NULL));
    std::uniform_int_distribution<int> random_c(1, 51);
    std::string test_str;

    int test_array_length = ARR_LEN(test_array);
    std::cout << test_array_length << std::endl;

    test_str.append("+++");
    for (int i = 0; i < test_array_length * 1; i++) {
        for (int j = 0; j < test_array_length; j++) {
            test_str.append(test_array[random_c(generator)]);
        }
    }
    test_str.append("---");
    return test_str;
}

std::string TestData::buildRandomTestTitle() {
    std::default_random_engine generator(time(NULL));
    std::uniform_int_distribution<int> random_c(1, 51);
    std::string test_str;
    int test_array_length = ARR_LEN(test_array);
    std::cout << test_array_length << std::endl;

    test_str.append("=TS=");
    for (int i = 0; i < test_array_length / 2; i++) {
        for (int j = 0; j < test_array_length; j++) {
            test_str.append(test_array[random_c(generator)]);
        }
    }
    test_str.append("=TE=");
    return test_str;
}

std::string TestData::getTestData() {
    return buildRandomTestData();
}