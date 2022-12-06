#include <version>
#include <algorithm>
#include <type_traits>
#include <map>
#include <set>
#include <vector>
#include <ostream>
#include <cassert>

//#define STD_RANGES 0

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

//#define TRACE_PRETTY
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

//template <class U>
//struct Less {
//	static_assert(!std::numeric_limits<U>::is_signed, "U must be unsigned");
//	bool operator()(U x, U y) const{
////		return isNewer(y, x);
//		constexpr U half = (std::numeric_limits<U>::max() >> 1) + 1;
//		if (y - x == half) {
//			return x < y;
//		}
//		return x != y && static_cast<U>(y - x) < half;
//	}
//};


using T = uint8_t;
//using Map = multimap<T, T, Less<T>>;

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

//#ifdef make_subrange
//#undef make_subrange
//#endif


struct S {
	S() = default;
	explicit S(int v) : x(v) {}
	S(const S& s) : x(s.x) { cout << '\n' << "S copy ctor\n"; }
	S(S&&) = default;
	int x = 9;
};
ostream& operator<<(ostream& os, const S& s) {
	return os << s.x;
}


using SP = unique_ptr<S>;
class XMap {
	bool less(T x, T y) const{
		constexpr T half = (std::numeric_limits<T>::max() >> 1) + 1;
		if (y - x == half) {
			return x < y;
		}
		return x != y && static_cast<T>(y - x) < half;
	}

public:
	using Map = std::multimap<T, SP>;
	using value_type =  Map::value_type;
	using It = Map::iterator;

	auto equal_range(T x) const {
		auto [b, e] = mp.equal_range(x);
		return rg::make_subrange(b, e);
	}

	auto subrange(T lo, T hi) {
		return vs::iota(0, 2) | vs::transform([this, lo, hi](auto k) {
			if (k == 0)
				return rg::make_subrange(mp.lower_bound(lo), hi >= lo ? mp.upper_bound(hi) : mp.end());
			else {
				return rg::make_subrange(mp.begin(), hi >= lo ? mp.begin() : mp.upper_bound(hi));
			}
		}) | vs::join;
	}

	auto all()  { return subrange(inf, sup); }

	auto lower_bound(T x) const { return mp.lower_bound(x); }
	void push(T key, SP&& value);
	void erase(It it);

	void erase(T key) {
//		auto [b, e] = mp.equal_range(key);
		auto b = mp.find(key);
		while (b != mp.end()) {
			erase(b);
			b = mp.find(key); // mp.equal_range(key).first;
		}
	}

	std::multimap<T, SP> mp;
	T inf;
	T sup;
};

void XMap::push(T key, SP&& value) {
	if (mp.empty()) {
		inf = sup = key;
	} else {
		if (less(key, inf)) inf = key;
		if (less(sup, key)) sup = key;
//		if (isNewer(inf, key)) inf = key;
//		if (isNewer(key, sup)) sup = key;
	}
	mp.insert({key, std::move(value)});
}

void XMap::erase(It it) {
	if (it == mp.end()) return;
	if (mp.size() == 1 || inf == sup) {
		// all keys are equal
		mp.erase(it);
		return;
	}
	// if we remove the first one (==inf) just advance `inf` to the next
	if (it->first == inf) {
		It p = it; ++p;
		if (p == mp.end())
			inf = mp.begin()->first; // wrap it aka overflow
		else
			inf = p->first;
		mp.erase(it);
	} else if (it->first == sup) {
		// need to find new sup
		// At FEC we only remove the oldest elements so this case is not expected
		if (inf <= sup) {
			// trivial: no wrap
			mp.erase(it);
			sup = mp.rbegin()->first;
		} else  if (mp.begin()->first == sup) {
			mp.erase(it);
			if (mp.begin()->first == sup) {
				// there are more elements with the same key; keep sup intact
			} else {
				// wrap it and take the last key
				sup = mp.rbegin()->first;
			}
		} else {
			// There are lower keys so let's find the highest one < sup (binary search)
			auto lo = mp.begin()->first;
			auto hi = sup;
			while (hi - lo > 1) {
				auto mid = lo + (hi - lo)/2;
				It up = mp.upper_bound(mid);
				assert(up != mp.end());
				if (up->first < sup) {
					lo = mid;
				} else {
					hi = mid;
				}
			}
			// hi may be better candidate; or hi may be = sup
			mp.erase(it);
			// check if key = sup still exists; otherwise set sup to prev
			if (mp.find(sup) == mp.end()) {
				// there is no such key anymore; move to previous
				if (sup != hi)
					sup = hi;
				else
					sup = lo;
			}
		}
	} else {
		mp.erase(it);
	}
}


std::ostream& operator<<(std::ostream& os, XMap::value_type const& v) {
	return os << '(' << (uint)v.first << " : " << (uint)v.second->x << ')';
}

std::ostream& operator<<(std::ostream& os, std::pair<T, SP> const& v) {
	return os << '(' << (uint)v.first << " : " << (uint)v.second->x << ')';
}

/// compare two ranges
bool equals(auto&& a, auto&& b) {
//	auto pa = a.begin();
//	auto pb = b.begin();
//	for ( ; pa != a.end() && pb != b.end(); ++pa, ++pb) {
//		TraceX(*pa, *pb);
//		if (*pa != *pb) return false;
//	}
//	if (pa != a.end() || pb != b.end()) return false;
//	return true;
	return rg::distance(a) == rg::distance(b) &&
		rg::all_of(vs::zip(a, b), [](auto&& pair){ return pair.first == pair.second; });
}

void test_equals() {
	assert( equals(vector<int>{1, 2, 3}, vector<int>{1, 2, 3}) );
	assert( !equals(vector<int>{1, 2, 3}, vector<int>{1, 2, 4}) );
	assert( !equals(vector<int>{1, 2, 3, 4}, vector<int>{1, 2, 3}) );

	assert( equals(set{-1,0,1}, set{-1,0,1}) );
	assert( !equals(set{-1,0,1}, set{-1,0,2}) );
	assert( !equals(set{-1,0,1}, set{-1,0,1,2}) );

	log_trace << "Test OK";
}

bool operator==(XMap::value_type const& x, XMap::value_type const& y) {
	return x.first == y.first && x.second == y.second;
}

bool operator==(XMap::value_type const& x, pair<T, T> const& y) {
	return x.first == y.first && x.second->x == y.second;
}

bool operator!=(XMap::value_type const& x, pair<T, T> const& y) {
	return !(x == y);
}

bool equalsV(auto&& m, vector<pair<T, T>> const& v) {
	return equals(m, v);
}

void test_XMap() {
	test_equals();

	XMap m;
	auto push = [&](T k, T v = 0) { m.push(k, make_unique<S>(v)); };

	for (T x : {1,2,3,4}) push(x);
	push(3,1); push(3,2);
	TraceX(m.mp);

	assert( equalsV(m.all(), {{1, 0}, {2, 0}, {3, 0}, {3, 1}, {3, 2}, {4, 0}}) );

	cout << '\n'; TraceX((uint)m.inf, (uint)m.sup, m.all());

	push(253); push(255); push(255, 255);
//	TraceX((uint)m.inf, (uint)m.sup);
//	for (auto& x : m.all()) TraceX(x);
//	TraceX(11);
	assert( equalsV(m.all(), {{253, 0}, {255, 0}, {255, 255}, {1, 0}, {2, 0}, {3, 0}, {3, 1}, {3, 2}, {4, 0}}) );

	TraceX((uint)m.inf, (uint)m.sup, m.all());

	m.erase(1);
	TraceX((uint)m.inf, (uint)m.sup, m.all());
	assert( equalsV(m.all(), {{253, 0}, {255, 0}, {255, 255}, {2, 0}, {3, 0}, {3, 1}, {3, 2}, {4, 0}}) );

	m.erase(42);
	TraceX((uint)m.inf, (uint)m.sup, m.all());
	assert( equalsV(m.all(), {{253, 0}, {255, 0}, {255, 255}, {2, 0}, {3, 0}, {3, 1}, {3, 2}, {4, 0}}) );

	m.erase(253);
	TraceX((uint)m.inf, (uint)m.sup, m.all());
	assert( equalsV(m.all(), {{255, 0}, {255, 255}, {2, 0}, {3, 0}, {3, 1}, {3, 2}, {4, 0}}) );

	m.erase(4);
	TraceX((uint)m.inf, (uint)m.sup, m.all());
	assert( equalsV(m.all(), {{255, 0}, {255, 255}, {2, 0}, {3, 0}, {3, 1}, {3, 2}}) );

	m.erase(3);
	TraceX((uint)m.inf, (uint)m.sup, m.all());
	assert( equalsV(m.all(), {{255, 0}, {255, 255}, {2, 0}}) );

	push(250);
	m.erase(2);
	TraceX((uint)m.inf, (uint)m.sup, m.all());
	assert( equalsV(m.all(), {{250, 0}, {255, 0}, {255, 255}}) );

	m.erase(255);
	TraceX((uint)m.inf, (uint)m.sup, m.all());
	assert( equalsV(m.all(), {{250, 0}}) );

	m.erase(255);
	assert( equalsV(m.all(), {{250, 0}}) );

	m.erase(250);
	TraceX((uint)m.inf, (uint)m.sup, m.all());
	assert( equalsV(m.all(), {}) );

	m.erase(0);
	assert( equalsV(m.all(), {}) );

	push(12); push(200);
	TraceX((uint)m.inf, (uint)m.sup, m.all());
	assert( equalsV(m.all(), {{200, 0}, {12, 0}}) );

	for (uint k = 245u; k < 265u; k+=2) {
		T u = (T)k;
		push(u);
	}
	push(250,250);
	push(0,1); push(0,2);
	TraceX((uint)m.inf, (uint)m.sup, m.all());

	TraceX(m.subrange(248, 253));
	assert( equalsV(m.subrange(248, 253), {{249, 0},{250, 250},{251, 0},{253, 0}}) );

	TraceX(m.subrange(253, 8));
	assert( equalsV(m.subrange(253, 8), {{253, 0},{255, 0},{0, 1},{0, 2},{1, 0},{3, 0},{5, 0},{7, 0}}) );
}


int main() {
//	testMap_Plain();
//	testMap();
//	cout << '\n';
	test_XMap();
	return 0;
}