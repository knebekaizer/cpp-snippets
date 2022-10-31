//
// Created by Vladimir on 31.10.2022.
//

#include "factory.hpp"

#include <string>
using namespace std;

template<>
Factory& Factory::instance() {
    static Factory self([](int&&, string&&){ return new Base{}; });
    return self;
}


void test() {
    int x = 42;
    string s = "42";
    auto node1 = Factory::create<0>(std::move(x), std::move(s));
    node1->foo();

    auto node12 = Factory::create<12>(0, ""s);
    node12->foo();

    auto node42 = Factory::create<42>(0, ""s);
    node42->foo();
}

int main() {
    test();
    return 0;
}

