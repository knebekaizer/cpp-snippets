//
// Created by vdi on 28.03.24.
//

#include "trace.hpp"
#include <iostream>
#include <cmath>
#include <random>
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

template<class G>
typename std::enable_if<std::is_same<G, std::ranlux24>::value ||
                            std::is_same<G, std::ranlux48>::value ||
                            std::is_same<G, std::knuth_b>::value,
                        G>::type
    create_generator() {
    TraceX(1, typeid(G).name());
    std::mt19937 gen(42);
    return G(gen());
}
template<class G>
typename std::enable_if<!std::is_same<G, std::ranlux24>::value &&
                            !std::is_same<G, std::ranlux48>::value &&
                            !std::is_same<G, std::knuth_b>::value,
                        G>::type
    create_generator() {
    TraceX(2, typeid(G).name());
    return G(std::random_device{}());
}

template <typename RE>
void test_random() {
    using F = float;
    std::uniform_real_distribution<F> distribution(1.0, 2.0);
    std::uniform_real_distribution<F>::param_type param(0, 42);
//    auto g = RE(random_device{}());
    auto g = create_generator<RE>();
    using Int = size_t;
    const Int N = 100000;
    for (int k = 0; k < 100; ++k)
    for (Int i = 0; i < N; ++i) {
        auto x = distribution(g, param);
        if (x >= param.b()) TraceX("FAILED", x, param.b());
        int n = static_cast<int>(x - param.a());
        if (n >= 42) TraceX(n, x);
        assert(n < 42);
    }
}

void testAllRandom() {
//    test_random<std::default_random_engine>();
//    test_random<std::knuth_b>();
    test_random<std::minstd_rand0>();
    test_random<std::minstd_rand>();
//    test_random<std::mt19937>();
//    test_random<std::mt19937_64>();
//    test_random<std::ranlux24>();
//    test_random<std::ranlux24_base>();
//    test_random<std::ranlux48>();
    test_random<std::ranlux48_base>();
}

int main() {
    testDouble();
    testF();
    for (int i = 0; i < 20; ++i) {
        testAllRandom();
    }
    return 0;
}