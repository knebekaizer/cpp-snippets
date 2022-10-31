//
// Created by Vladimir on 31.10.2022.
//

#include "factory.hpp"

Factory& Factory::instance() {
    static Factory self([](){ return new Base{}; });
    return self;
}


void test() {
    auto node1 = Factory::create<0>();
    node1->foo();

    auto node12 = Factory::create<12>();
    node12->foo();

    auto node42 = Factory::create<42>();
    node42->foo();
}

int main() {
    test();
    return 0;
}

