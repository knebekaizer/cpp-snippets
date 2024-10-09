//
// Created by Vladimir on 2024-06-23.
//

#include <limits>
#include <bitset>
#include <string.h>
#include <math.h>
#include <string>

#include "trace.hpp"
using namespace std;

template <typename T>
bool equalBits(const T& f1, const T& f2) {
	return 0 == memcmp(&f1, &f2, sizeof(T));
}

void test_inf() {
	using T = double;
	cout << boolalpha << setprecision(20);

	auto Max = std::numeric_limits<T>::max();
	auto Min = std::numeric_limits<T>::min();
	auto Inf = std::numeric_limits<T>::infinity();
	TraceX(Max, Min, Inf);
	TraceX(Inf > Max);  // true
	TraceX(Max + Min);
	TraceX(Max == (Max + Min)); // true

	cout << endl;
	log_trace << "m2 = nextafter(Max, Inf):";
	auto m2 = nextafter(Max, Inf);
	TraceX(Max, m2);
	TraceX(m2 > Max);
	TraceX(m2 == Inf);
	TraceX(m2 == (Max + Min));

	cout << endl;
	log_trace<< "NaN Arithmetics:";
	TraceX(Max * 2);    // inf
	TraceX(Max / 0.0);  // inf
	TraceX((Max / 0.0) == (Max * 2));   // true
	TraceX(sqrt(-1.0)); // nan
	TraceX(acos(2));    // nan
	TraceX(acos(2) == sqrt(-1.0));  // false
	TraceX(equalBits(acos(2), sqrt(-1.0))); // true

	auto Nan = std::numeric_limits<T>::quiet_NaN();
	auto Nan2 = std::numeric_limits<T>::quiet_NaN();
	auto Nan0 = std::numeric_limits<T>::signaling_NaN();
	auto Nan1 = Nan;
	TraceX(Nan, Nan2, Nan0);
	TraceX((uint64_t)Nan == (uint64_t)Nan1);
	TraceX(Nan == Nan1);    // false
	TraceX(Nan == Nan1);    // false
	TraceX(equalBits(Nan, Nan2));   // true
	TraceX(equalBits(Nan, Nan0));   // false
	TraceX(Nan > Max); // false
	TraceX(Nan > Inf); // false
}


#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstrict-aliasing"
#include <cfloat>
#include <limits>
void test_div() {
    // a = 0xBD4A3FFFFFFFFFFFULL; b = 0x80000000000000D2ULL; the algorithm returns 0x7ff0000000000000ULL
    cout << boolalpha;
    {
        uint64_t ai = 0xBD4A3FFFFFFFFFFFull;
        uint64_t bi = 0x80000000000000D2ull;
        double a = *(double*)&ai;
        double b = *(double*)&bi;
        double c = a / b;
        printf("c = %lx\n", *(uint64_t*)&c); // DBL_MAX = 7fefffffffffffff
        TraceX(c == numeric_limits<double>::max());
    }
    {
        uint64_t ai = 0xBD4A3FFFFFFFFFFFull;
        uint64_t bi = 0x80000000000000D1ull;
        double a = *(double*)&ai;
        double b = *(double*)&bi;
        double c = a / b;
        printf("c = %lx\n", *(uint64_t*)&c); // INF = 7ff0000000000000
        TraceX(c == numeric_limits<double>::infinity());
    }
}
#pragma GCC diagnostic pop


void test_NAN() {
    auto quiet_NaN = numeric_limits<float>::quiet_NaN();
    auto signaling_NaN = numeric_limits<float>::signaling_NaN();
    TraceX(quiet_NaN == signaling_NaN);
    hash<float> h;
    TraceX(h(quiet_NaN) == h(signaling_NaN));
}

int main() {
	TraceX(sizeof(long double));

	test_inf();
    test_div();
    test_NAN();
	return 0;
}