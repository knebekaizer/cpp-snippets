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

size_t seed;
template<class G>
auto repro_gen() {
    std::mt19937 dev(++seed);
    return G(dev());
}

struct Min {
	size_t seed = 0;
	size_t iter = numeric_limits<decltype(iter)>::max();
} stat;

template <typename RE>
int test_random() {
    using F = float;
    std::uniform_real_distribution<F>::param_type param(0, 2);
    std::uniform_real_distribution<F> distribution(param);
//    auto g = RE(random_device{}());
//    auto g = RE{std::random_device{}()}; //create_generator<RE>();
    auto g = repro_gen<RE>();
    const size_t N = 2000000;
//    for (size_t k = 0; k < 100; ++k)
    for (size_t i = 0; i < N; ++i) {
        auto x = distribution(g);
        if (x >= distribution.max()) {
			printf("FAILED! seed: %lu, iterations: %lu; %f, %f\n", seed, i, x, distribution.max());
	        if (i < stat.iter) stat = {seed, i};
	        return -1;
        }
    }
    return 0;
}


int testAllRandom() {
//    test_random<std::default_random_engine>();
//    test_random<std::knuth_b>();
//    test_random<std::minstd_rand0>();
//    if (test_random<std::minstd_rand>()) return 1;
    test_random<std::minstd_rand>();
//    test_random<std::mt19937>();
//    test_random<std::mt19937_64>();
//    test_random<std::ranlux24>();
//    test_random<std::ranlux24_base>();
//    test_random<std::ranlux48>();
//    test_random<std::ranlux48_base>();
    return 0;
}

int main() {
    for (int i = 0; i < 100; ++i) {
        if (testAllRandom()) break;
    }
	TraceX(stat.seed, stat.iter);
	return 0;
}