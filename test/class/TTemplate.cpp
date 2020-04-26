//
// Created by torment on 2020/4/26.
//

#include "TTemplate.h"

namespace class_test {
    template<typename T, typename F>
    void TTemplate<T, F>::print(T t, F f) {

    }
}

int main() {
    class_test::TTemplate<wchar_t, wchar_t> *tt = new class_test::TTemplate<wchar_t, wchar_t>();
    tt->print(123, 123);
}