# include "PriorityQueue.hpp"

#include <iostream>
#include <random>
#include <utility>

#include "trace.h"

using namespace std;

struct Node {
	int key;
	int value;
};

auto operator<(const Node& a, const Node& b) { return a.key < b.key; }
auto operator==(const Node& a, const Node& b) { return a.key == b.key; }

ostream& operator<<(ostream& os, const Node& n) { return os << n.key << ":" << n.value; }

auto test_data(int Max) {
	default_random_engine gen{};
	uniform_int_distribution dist(0, Max);
	for (auto k=0; k<32; ++k) cout << dist(gen) << " "; cout << endl;
}


auto test(int Max) {
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

vector<Node> testData(int Max = 9) {
	const int N = 32;
	const int seed = 42;
	default_random_engine gen(seed);
	uniform_int_distribution dist(0, Max);
	vector<Node> v;
	v.reserve(N);
	for (int k=0; k!=N; ++k) {
//		v.emplace_back(dist(gen), k);
		v.push_back({dist(gen), k});
	}
	return v;
}

void testNodeOwn(Ordering ord = Ordering::NONE) {
	PriorityQueue<Node> a;
	for (const auto& x : testData()) {
		a.insert(x, ord);
	}
	log_trace << a.dump();
}

void testNodeStd(Ordering ord = Ordering::NONE) {
	PriorityQueue<Node> a;
	for (const auto& x : testData()) {
		a.insert_std(x, ord);
	}
	log_trace << a.dump();
}

int main() {
	std::cout << "Hello, World!" << std::endl;
	test_data(9);
	test(9);
	testNodeOwn();
	testNodeStd();

	log_trace << "FIFO";
	testNodeOwn(Ordering::FIFO);
	testNodeStd(Ordering::FIFO);

	log_trace << "LIFO";
	testNodeOwn(Ordering::LIFO);
	testNodeStd(Ordering::LIFO);

	return 0;
}
