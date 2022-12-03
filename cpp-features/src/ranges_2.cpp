#include <version>
#include <algorithm>
#include <type_traits>
#include <map>
#include <ostream>

#define STD_RANGES 0

#ifndef STD_RANGES
#ifdef __cpp_lib_ranges
#define STD_RANGES 1
#else
#define STD_RANGES 0
#endif
#endif

#if defined __clang__
std::ostream& operator<<(std::ostream& os, std::pair<uint16_t, uint16_t> const& v);
#endif

#include "range_print.hpp"


#if STD_RANGES
#include <ranges>
namespace rg = std::ranges;
#else
#include "range/v3/all.hpp"
namespace rg = ::ranges;
#endif // __cpp_lib_ranges
namespace vs = rg::views;

#define TRACE_PRETTY
#include "trace.hpp"

using namespace std;


std::ostream& operator<<(std::ostream& os, std::pair<uint16_t, uint16_t> const& v) {
	return os << '(' << (uint)v.first << " : " << (uint)v.second << ')';
}

using T = uint8_t;
using Map = multimap<T, T>;

struct S {
	S() = default;
	S(const S& s) : x(s.x) { cout << '\n' << "S copy ctor\n"; }
	S(S&&) = default;
	int x = 9;
};
ostream& operator<<(ostream& os, const S& s) {
	return os << s.x;
}

void test_vector() {
	vector<S> sv(5);
	// Check this dos not call copy ctor
	TraceX(sv);
	TraceX(sv | vs::all);
}

#if STD_RANGES
#define make_subrange subrange
#endif

auto to_rng(auto p) {
	auto [b, e] = p;
	return rg::make_subrange(b, e);
}

auto eq_range(const auto& m, T x) {
	return to_rng(m.equal_range(x));
}

#ifdef make_subrange
#undef make_subrange
#endif

void testMap() {
	Map m;
	[[maybe_unused]] auto push = [&](T x) {
		if (!m.empty() && uint(x - m.begin()->first) > 64) {
			m.erase(m.begin(), m.end());
		}
		m.insert({x,x});
	};

	for (T x : {1,2,3}) push(x); TraceX(m);
	for (T x : {200, 1,2}) push(x); TraceX(m);
	for (T x : {10, 20, 30, 1,2}) push(x); TraceX(m);
	for (T x : {40,50,60,70,80,90}) push(x); TraceX(m);
	m.insert({80, 81});
	m.insert({80, 82});


#if STD_RANGES
	auto map_subrange = rg::subrange(m.begin(), m.end());
#else
	[[maybe_unused]]
	auto map_subrange = rg::make_subrange(m.begin(), m.end());
#endif
	TraceX(m);
	TraceX(map_subrange);

	TraceX(eq_range(m, 40));
	TraceX(eq_range(m, 80));
	TraceX(eq_range(m, 200));
	TraceX(*m.find(90));
	TraceX(*m.lower_bound(80));
	TraceX(*m.upper_bound(80));

}

int main() {
	test_vector();
	testMap();
	return 0;
}