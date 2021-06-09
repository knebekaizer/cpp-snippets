# include "PriorityQueue.hpp"

#include <iostream>
#include <random>

#include "trace.h"

using namespace std;
auto test_std(int Max) {
	const int N = 32;
	const int seed = 42;
	default_random_engine gen;
	uniform_int_distribution dist(0, Max);
	{
		PriorityQueue<int> a;
		gen.seed(seed);
		for (auto k = 0; k < N; ++k) {
			auto v = dist(gen);
			cout << v << " ";
			a.insert(v);
		}
		cout << endl;
		log_trace << a.dump();
	}
	{
		PriorityQueue<int> a;
		gen.seed(seed);
		for (auto k = 0; k < N; ++k) {
			auto v = dist(gen);
			a.insert_std(v);
		}
		log_trace << a.dump();
	}
}
