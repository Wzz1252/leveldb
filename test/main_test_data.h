//
// Created by torment on 2020/4/2.
//

#define ARR_LEN(arr) sizeof(arr) / sizeof(arr[0])

#include <iostream>
#include <cstdlib>
#include <random>

class TestData {
public:
    TestData() = default;

    std::string buildRandomTestData();

    std::string buildRandomTestTitle();

    std::string getTestData();
};