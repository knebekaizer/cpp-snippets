#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <map>
#include <random>
#include <cmath>
#include "trace.h"

using namespace std;

void dump_random(ostream& os, int stdDev) {
	// Seed with a real random value, if available
	std::random_device r;

	std::mt19937 e3(r());
	int mean = 0;
	std::normal_distribution<> normal(mean, stdDev);
	int constexpr N = 512 * 512 * 32;

	for (int k = 0; k < N; ++k) {
		char x = (char)round(normal(e3));
		os << x;
	}
}

// sparse
void dump2(ostream& os, int stdDev) {
	// Seed with a real random value, if available
	std::random_device r;

	std::mt19937 e3(r());
	int mean = 0;
	std::normal_distribution<> normal(mean, stdDev / 2);
	int constexpr N = 512 * 512 * 32;

	for (int k = 0; k < N; ++k) {
		char x = (char)(2 * round(normal(e3)));
		os << x;
	}
}
void randomFloat(unsigned seed)
{
	std::mt19937 mt;
	mt.seed(seed);
	std::uniform_real_distribution<float> dt(-1,1);
	for (int k = 0; k < 5; ++k) cout << dt(mt) << " ";
	cout << endl;
//	return dt(mt);
}
int main()
{
	log_info << "Start";

	// Seed with a real random value, if available
	std::random_device r;

	// Choose a random mean between 1 and 6
	std::default_random_engine e1(r());
	std::uniform_int_distribution<int> uniform_dist(1, 6);
	int mean = uniform_dist(e1);
	std::cout << "Randomly-chosen mean: " << mean << '\n';

	// Generate a normal distribution around that mean
	std::seed_seq seed2{r(), r(), r(), r(), r(), r(), r(), r()};
	std::mt19937 e2(seed2);
	std::normal_distribution<> normal_dist(mean, 2);

	std::map<int, int> hist;
	for (int n = 0; n < 10000; ++n) {
		++hist[std::round(normal_dist(e2))];
	}
	std::cout << "Normal distribution around " << mean << ":\n";
	for (auto p : hist) {
		std::cout << std::fixed << std::setprecision(1) << std::setw(2)
				  << p.first << ' ' << std::string(p.second/200, '*') << '\n';
	}

	{
		std::mt19937 e3(r());
		std::normal_distribution<> normal(100, 8);
		for (int k = 0; k < 10; ++k) {
			cout << (int)round(normal(e3)) << " ";
		}
		cout << endl;
	}
	{
		std::mt19937 e3;
		std::normal_distribution<> normal(100, 8);
		for (int k = 0; k < 10; ++k) {
			cout << (int)round(normal(e3)) << " ";
		}
		cout << endl;
	}
	ofstream o8("o8");
	dump_random(o8, 8);

	ofstream o82("o82");
	dump2(o82, 8);

	ofstream o64("o64");
	dump_random(o64, 64);

	randomFloat(1);
	randomFloat(12);
	randomFloat(12);

	return 0;
}
