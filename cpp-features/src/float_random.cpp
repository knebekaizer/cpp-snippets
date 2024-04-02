#include <iostream>
#include <random>
using namespace std;

template <typename RE>
int testSingle(size_t seed) {
	std::uniform_real_distribution<float> distribution(0.0f, 2.0f);
	auto g = RE(seed);
	const size_t N = 2000000;
    cout << boolalpha;
	for (size_t i = 0; i < N; ++i) {
		auto x = distribution(g);
		if (x >= distribution.b()) {
			cout << "FAILED! seed: " << seed << "; iterations: " << i << "; value: " << x
                 << "; max: " << distribution.b()
                 << "; (b-x): " << (distribution.b() - x)
                 << "; (x-a): " << (x - distribution.a())
                 << "; int(x-a): " << (int)(x - distribution.a())
                 << endl;
			return -1;
		}
        if ((is_same<RE, std::mt19937>::value && seed == 3 && i == 1414715)
            || (is_same<RE, std::minstd_rand>::value && seed == 25 && i == 689064)) {
            cout << "PASSED! seed: " << seed << "; iterations: " << i << "; value: " << x
                 << "; max: " << distribution.b()
                 << "; x < b: " << (x < distribution.b())
                 << "; b-x = " << (distribution.b() - x)
                 << "; x-a = " << (x - distribution.a())
                 << "; int(x-a) = " << (int)(x - distribution.a())
                 << endl;
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