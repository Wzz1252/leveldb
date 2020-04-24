//
// Created by torment on 2020/4/21.
//

#include "TClass.h"
#include <iostream>

using namespace std;

class People; // 仅仅是声明一个类

// People 仅仅是声明，所以下面的 People2 不能继承自 People
// class People2 : public People {
// };

class Employee {
public:
    virtual void print() const;
};

void Employee::print() const {
    std::cout << "Employee print" << std::endl;
}

class Manager : public Employee {
public:
    void print() const override;
};

void Manager::print() const {
    std::cout << "Manager print" << std::endl;
    Employee::print();
}

int main() {
    Employee e;
    e.print();
    Manager m;
    m.print();

    return 1;
}