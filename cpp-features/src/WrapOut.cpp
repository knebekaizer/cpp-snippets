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
	bool operator()(T x, T y) const{ return isNewer(y, x); }
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
	TraceX(*m.lower_bound(200));
	TraceX(*m.upper_bound(200));
}

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
public:
	using Map = std::multimap<T, SP>;
	using It = Map::iterator;
	auto equal_range(T x) const { return mp.equal_range(x); }
	Map::const_iterator lower_bound(T x) const;

	auto all()  {
		It p = mp.begin();
		if (sup < inf) {
			p = mp.equal_range(inf).first;
		}
		return vs::iota(0u, mp.size()) | vs::transform([p, this](auto k) mutable-> Map::value_type& {
			auto it = p++;
			if (p == mp.end()) p = mp.begin();
			return *it;
		});
	}

//	auto all2()  {
//		if (sup >= inf) {
//			return rg::make_subrange(mp.begin(), mp.end());
//		} else {
//			auto [b, _] = mp.equal_range(inf);
//			auto e = mp.upper_bound(sup);
//			return vs::iota(0, 2) | vs::transform([&](auto k) {
//				if (k == 0)
//					return rg::make_subrange(b, mp.end());
//				else
//					return rg::make_subrange(mp.begin(), e);
//			}) | vs::join;
//		}
//	}

//	static bool less(T a, T b) { return isNewer(b, a); }
	void push(T key, SP&& value) {
		if (mp.empty()) {
			inf = sup = key;
		} else {
			if (isNewer(inf, key)) inf = key;
			if (isNewer(key, sup)) sup = key;
		}
		mp.insert({key, std::move(value)});
	}
	void erase(It it) {
		if (it == mp.end()) return;
		if (mp.size() > 1 && inf != sup) {
			if (it->first == inf) {
				It p = it; ++p;
				if (p == mp.end())
					inf = mp.begin()->first;
				else
					inf = p->first;
				mp.erase(it);
			} else if (it->first == sup) {
				mp.erase(it);
				It p = mp.lower_bound(sup);
				if (p == mp.end()) {
					p = mp.lower_bound(std::numeric_limits<T>::max());
					assert(p != mp.end());
				}
				sup = p->first;
			} else {
				mp.erase(it);
			}
		} else {
			mp.erase(it);
		}
	}

	void erase(T key) {
//		auto [b, e] = mp.equal_range(key);
		auto b = mp.find(key);
		while (b != mp.end()) {
			erase(b);
			TraceX(mp.size());
			b = mp.find(key); // mp.equal_range(key).first;
		}
//		for (auto [b, e] = mp.equal_range(key); b != mp.end() && b != e; ++b) TraceX((uint)b->first); // erase(b);
	}

	std::multimap<T, SP> mp;
	T inf;
	T sup;
};

XMap::Map::const_iterator XMap::lower_bound(T x) const {
	return mp.cend();
}


std::ostream& operator<<(std::ostream& os, XMap::Map::value_type const& v) {
	return os << '(' << (uint)v.first << " : " << (uint)v.second->x << ')';
}

std::ostream& operator<<(std::ostream& os, std::pair<T, SP> const& v) {
	return os << '(' << (uint)v.first << " : " << (uint)v.second->x << ')';
}

void test_XMap() {
	XMap m;
	auto push = [&](T k, T v = 0) { m.push(k, make_unique<S>(v)); };
	for (T x : {1,2,3,4}) push(x);
	push(3,1); push(3,2);
	TraceX(m.mp);

	TraceX((uint)m.inf, (uint)m.sup);
	TraceX(m.all());
	cout << '\n';

	push(253); push(255); push(255, 255);
	TraceX((uint)m.inf, (uint)m.sup);
	TraceX(m.all());
	cout << '\n';

	m.erase(1);
	TraceX(1, (uint)m.inf, (uint)m.sup);
	TraceX(m.all());
	cout << '\n';

	m.erase(42);
	TraceX(42, (uint)m.inf, (uint)m.sup);
	TraceX(m.all());
	cout << '\n';

	m.erase(253);
	TraceX(253, (uint)m.inf, (uint)m.sup);
	TraceX(m.all());
	cout << '\n';

	m.erase(4);
	TraceX(4, (uint)m.inf, (uint)m.sup);
	TraceX(m.all());
	cout << '\n';

	m.erase(3);
	TraceX(3, (uint)m.inf, (uint)m.sup);
	TraceX(m.all());
	cout << '\n';

}


int main() {
	testMap_Plain();
	testMap();
	cout << '\n';
	test_XMap();
	return 0;
}