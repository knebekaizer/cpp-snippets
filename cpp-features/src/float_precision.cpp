//
// Created by vdi on 28.03.24.
//

#include "trace.hpp"
#include <iostream>
#include <cmath>
#include <limits>
#include <cassert>
using namespace std;

void testDouble() {
    using F = long double;
    F a = 0;
    F b = 42;
    F x = nextafter(b, a);
    cout << boolalpha;
    TraceX(x < b);
    TraceX((int32_t)(x - a));
}

void testF() {
    using Int = int32_t;
    const Int N = 100000;
    for (Int i = 0; i < N; ++i) {
        auto x = (double)i;
        if ((Int)x != i) TraceX(i, x);
    }
}

int main() {
    testDouble();
    testF();
    return 0;
}