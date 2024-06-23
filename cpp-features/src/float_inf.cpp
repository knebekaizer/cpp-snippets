//
// Created by Vladimir on 2024-06-23.
//

#include <limits>
#include <bitset>
#include <string.h>
#include <math.h>

#include "trace.hpp"
using namespace std;

template <typename T>
bool equalBits(const T& f1, const T& f2) {
	return 0 == memcmp(&f1, &f2, sizeof(T));
}

void test_inf() {
	using T = double;
	auto Max = std::numeric_limits<T>::max();
	auto Inf = std::numeric_limits<T>::infinity();
	cout << boolalpha << setprecision(20);
	TraceX(Inf > Max);
	TraceX(Inf);

	auto m2 = nextafter(Max, Inf);
	TraceX(Max, m2);
	TraceX(m2 > Max);
	TraceX(m2 == Inf);

	TraceX(Max * 2);
	TraceX(Max / 0.0);
	TraceX(sqrt(-1.0));
	TraceX(acos(2));
	TraceX(equalBits(acos(2), sqrt(-1.0)));

	auto Nan = std::numeric_limits<T>::quiet_NaN();
	auto Nan2 = std::numeric_limits<T>::quiet_NaN();
	auto Nan0 = std::numeric_limits<T>::signaling_NaN();
	auto Nan1 = Nan;
	TraceX(Nan, Nan2, Nan0);
	TraceX((uint64_t)Nan == (uint64_t)Nan1);
	TraceX(Nan == Nan1);
	TraceX(Nan == Nan1);
	TraceX(equalBits(Nan, Nan2));
	TraceX(equalBits(Nan, Nan0));
	TraceX(Nan > Max);
	TraceX(Nan > Inf);
}

int main() {
	TraceX(sizeof(long double));

	test_inf();
	return 0;
}