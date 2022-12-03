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


// TODO rework me
template<typename U>
inline bool isNewer(U value, U prev_value) {
	static_assert(!std::numeric_limits<U>::is_signed, "U must be unsigned");
	// kBreakpoint is the half-way mark for the type U. For instance, for a
	// uint16_t it will be 0x8000, and for a uint32_t, it will be 0x8000000.
	constexpr U kBreakpoint = (std::numeric_limits<U>::max() >> 1) + 1;
	// Distinguish between elements that are exactly kBreakpoint apart.
	// If t1>t2 and |t1-t2| = kBreakpoint: IsNewer(t1,t2)=true,
	// IsNewer(t2,t1)=false
	// rather than having IsNewer(t1,t2) = IsNewer(t2,t1) = false.
	if (value - prev_value == kBreakpoint) {
		return value > prev_value;
	}
	return value != prev_value && static_cast<U>(value - prev_value) < kBreakpoint;
}

template <class T>
struct Less {
	bool operator()(T x, T y) const{ return !isNewer(x, y); }
};
using T = uint8_t;
using Map = multimap<T, T, Less<T>>;

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
	TraceX(*m.lower_bound(75));
	TraceX(*m.upper_bound(75));
	TraceX(*m.lower_bound(80));
	TraceX(*m.upper_bound(80));
	TraceX(*m.lower_bound(85));
	TraceX(*m.upper_bound(85));
	TraceX(*m.lower_bound(200));
	TraceX(*m.upper_bound(200));
}

void testMap_Plain() {
	using Map = multimap<T, T>;
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
	TraceX(*m.lower_bound(75));
	TraceX(*m.upper_bound(75));
	TraceX(*m.lower_bound(80));
	TraceX(*m.upper_bound(80));
	TraceX(*m.lower_bound(85));
	TraceX(*m.upper_bound(85));
	TraceX(*m.lower_bound(200));
	TraceX(*m.upper_bound(200));
}

int main() {
	test_vector();
	testMap_0();
	testMap();

	testMap_Plain();
	return 0;
}