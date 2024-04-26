//
// Created by vdi on 28.03.24.
//

#include <bitset>
#include <cassert>
#include <climits>
#include <cmath>
#include <iostream>
#include <limits>
#include <random>
#include <typeinfo>
#include "trace.hpp"
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

//string floatToBin2(float f) {
//    constexpr unsigned int Size = sizeof(f) * CHAR_BIT;
//    bitset<Size> bits{*reinterpret_cast<unsigned int*>(&f)};
//    return bits.to_string();
//}

string floatToBin(float f) {
    auto* buf = (const unsigned char*)&f;
    string s;
    for (auto k = sizeof(f); k-->0; ++buf)
        s += bitset<8>{*buf}.to_string();
    return s;
}

void typesRepresentation() {
    TraceX(typeid(short).name(), typeid(unsigned short).name(), sizeof(short));
    TraceX(typeid(int).name(), typeid(unsigned int).name(), sizeof(int));
    TraceX(typeid(long).name(), typeid(unsigned long).name(), sizeof(long));
    TraceX(typeid(long long).name(), typeid(unsigned long long).name(), sizeof(long long));
    TraceX(floatToBin(.0f));
    TraceX(floatToBin(1.0f));
    TraceX(floatToBin(2.0f));
//    TraceX(floatToBin2(2.0f));
    TraceX(floatToBin(4.0f));
    TraceX(floatToBin(42.0f));
//    TraceX(floatToBin2(42.0f));
    TraceX(floatToBin(43.0f));
    TraceX(numeric_limits<float>::digits);
}

int main() {
//    typesRepresentation();
    testDouble();
    testF();
    for (int i = 0; i < 20; ++i) {
        testAllRandom();
    }
    return 0;
}