#include <iostream>
#include <random>
using namespace std;


template <typename RE>
int testSingle(size_t seed) {
	std::uniform_real_distribution<float> distribution(0.0f, 2.0f);
	auto g = RE(seed);
	const size_t N = 2000000;
	for (size_t i = 0; i < N; ++i) {
		auto x = distribution(g);
		if (x >= distribution.max()) {
			cout << "FAILED! seed: " << seed <<
					"; iterations: " << i <<
					"; value: " << x <<
					"; max: " << distribution.max() << endl;
			return -1;
		}
	}
	return 0;
}

template <typename RE>
int testRandom() {
	for (size_t seed = 0; seed < 100; ++seed) {
		if (testSingle<RE>(seed)) return 1;
	}
	cout << "PASSED!\n";
	return 0;
}

#define testRandom(engine) {cout << #engine << ": "; testRandom<engine>(); }

int testAllRandom() {
	testRandom(std::knuth_b);
    testRandom(std::minstd_rand0);
	testRandom(std::minstd_rand);
	testRandom(std::mt19937);
    testRandom(std::mt19937_64);
    testRandom(std::ranlux24);
    testRandom(std::ranlux24_base);
    testRandom(std::ranlux48);
    testRandom(std::ranlux48_base);
    return 0;
}


int main() {
    testAllRandom();
	return 0;
}