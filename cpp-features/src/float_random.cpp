//
// Created by vdi on 28.03.24.
//

#include "trace.hpp"
//#include <cassert>
#include <stdio.h>
#include <random>
using namespace std;

//template<class G>
//typename std::enable_if<std::is_same<G, std::ranlux24>::value ||
//                            std::is_same<G, std::ranlux48>::value ||
//                            std::is_same<G, std::knuth_b>::value,
//                        G>::type
//    create_generator() {
//    TraceX(1, typeid(G).name());
//    std::mt19937 gen(42);
//    return G(gen());
//}
//template<class G>
//typename std::enable_if<!std::is_same<G, std::ranlux24>::value &&
//                            !std::is_same<G, std::ranlux48>::value &&
//                            !std::is_same<G, std::knuth_b>::value,
//                        G>::type
//    create_generator() {
//    TraceX(2, typeid(G).name());
//    return G(std::random_device{}());
//}

int seed;
template<class G>
auto repro_gen() {
    std::mt19937 dev(++seed);
    return G(dev());
}

template <typename RE>
int test_random() {
    using F = float;
    std::uniform_real_distribution<F>::param_type param(0, 42);
    std::uniform_real_distribution<F> distribution(param);
//    auto g = RE(random_device{}());
//    auto g = RE{std::random_device{}()}; //create_generator<RE>();
    auto g = repro_gen<RE>();
    const size_t N = 100000;
    for (size_t k = 0; k < 100; ++k)
    for (size_t i = 0; i < N; ++i) {
        auto x = distribution(g);
        if (x >= param.b()) printf("FAILED! seed: %d, iterations: %lu:%lu; %f, %f\n", seed, k, i, x, param.b());
        int n = static_cast<int>(x - param.a());
        if (n >= 42) {
            printf("INT: %d, %f\n", n, x);
            TraceX(distribution.min(), distribution.max());
            return -1;
        }
        if (seed == 10 && k == 16 && i == 57022) {
            printf("PASSED? seed: %d, iterations: %lu:%lu; %f, %f\n", seed, k, i, x, param.b());
        }
    }
    return 0;
}

int testAllRandom() {
//    test_random<std::default_random_engine>();
//    test_random<std::knuth_b>();
//    test_random<std::minstd_rand0>();
    if (test_random<std::minstd_rand>()) return 1;
//    test_random<std::mt19937>();
//    test_random<std::mt19937_64>();
//    test_random<std::ranlux24>();
//    test_random<std::ranlux24_base>();
//    test_random<std::ranlux48>();
//    test_random<std::ranlux48_base>();
    return 0;
}

int main() {
    for (int i = 0; i < 20; ++i) {
        if (testAllRandom()) break;
    }
    return 0;
}