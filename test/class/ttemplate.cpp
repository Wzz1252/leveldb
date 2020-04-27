//
// Created by torment on 2020/4/26.
//

#include "ttemplate.h"

namespace class_test {
    template<typename T, typename F>
    void ttemplate<T, F>::print(T t, F f) {
        std::cout << "t+f: " << t + f << std::endl;
    }
}

int main() {
    auto *tt = new class_test::ttemplate<int, int>();
    tt->print(100, 200);
}