#include "trace.hpp"

#include <limits>
using namespace std;

// g++ --std=c++20 -I ../include/ signed_unsigned.cpp
void signed_overflow() {
	auto f = [](int x){ return x + 1 > x; };
	TraceX(f(1));   // gcc: 1; clang: 1
	TraceX(f(numeric_limits<int>::max()));   // gcc: 1; clang: 0
}

int main() {
	signed_overflow();
	return 0;
}