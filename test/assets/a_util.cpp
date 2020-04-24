//
// Created by torment on 2020/4/7.
//

#include "a_util.h"

namespace assets {

    void a_util::exponentiation(double value, int power, double &result) {
        double v = value;
        for (int i = 0; i < power - 1; i++) v = v * value;
        result = v;
    }

}