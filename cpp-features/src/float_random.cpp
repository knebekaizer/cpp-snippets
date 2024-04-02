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


struct Stat {
	size_t seed = 0;
	size_t iter = numeric_limits<decltype(iter)>::max();
} stat;

template <typename RE>
int testSingle(size_t seed) {
	using F = float;
	std::uniform_real_distribution<F> distribution(0.0f, 2.0f);
//    auto g = RE(random_device{}());
//    auto g = RE{std::random_device{}()}; //create_generator<RE>();
	auto g = RE(seed);
	const size_t N = 2000000;
	for (size_t i = 0; i < N; ++i) {
		auto x = distribution(g);
		if (x >= distribution.max()) {
			printf("FAILED! seed: %lu, iterations: %lu; value: %f, max: %f\n", seed, i, x, distribution.max());
			if (i < stat.iter) stat = {seed, i};
			return -1;
		}
	}
	return 0;
}

template <typename RE>
int testRandom(const char* name) {
	cout << name << ": ";
	for (size_t seed = 0; seed < 100; ++seed) {
		if (testSingle<RE>(seed)) return 1;
	}
	return 0;
}

template <typename RE>
int testRandom() {
	cout << typeid(RE).name() << ": ";
	for (size_t seed = 0; seed < 100; ++seed) {
		if (testSingle<RE>(seed)) return 1;
	}
	return 0;
}

//#define testRandom(engine) testRandom<engine>(#engine)

int testAllRandom() {
//    testRandom<std::default_random_engine>();
//	testRandom(knuth_b);
//    testRandom<std::minstd_rand0>();
	testRandom<std::minstd_rand>();
	testRandom<std::mt19937>();
    testRandom<std::mt19937_64>();
    testRandom<std::ranlux24>();
//    testRandom<std::ranlux24_base>();
//    testRandom<std::ranlux48>();
//    testRandom<std::ranlux48_base>();
    return 0;
}

int main() {
    testAllRandom();
return 0;
	log_trace << "minstd_rand";
    for (size_t seed = 0; seed < 100; ++seed) {
        testSingle<minstd_rand>(seed);
    }
	TraceX(stat.seed, stat.iter);

	log_trace << "mt19937";
	stat = Stat{};
	for (size_t seed = 0; seed < 100; ++seed) {
//        if (testSingle<mt19937>(seed)) break;
        testSingle<mt19937>(seed);
    }
	TraceX(stat.seed, stat.iter);

	{
		log_trace << "mt19937";
		stat = Stat{};
		random_device e;
		for (size_t k = 0; k < 100; ++k) {
			auto seed = e();
			testSingle<mt19937>(seed);
		}
		TraceX(stat.seed, stat.iter);
	}
	log_trace << "mt19937";
	stat = Stat{};
	default_random_engine e;
    for (size_t k = 0; k < 100; ++k) {
		auto seed = e();
        testSingle<mt19937>(seed);
    }
	TraceX(stat.seed, stat.iter);
	return 0;

	log_trace << "knuth_b";
	stat = Stat{};
	for (size_t seed = 0; seed < 100; ++seed) {
		testSingle<knuth_b>(seed);
	}
	TraceX(stat.seed, stat.iter);
	log_trace << "ranlux48";
	stat = Stat{};
	for (size_t seed = 0; seed < 100; ++seed) {
		testSingle<ranlux48>(seed);
	}
	TraceX(stat.seed, stat.iter);
	return 0;
}