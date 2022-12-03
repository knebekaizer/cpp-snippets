/// @file Experiments with `boost/circular_buffer`

#include "boost/circular_buffer.hpp"
#include <algorithm>
#include <type_traits>
#include <map>

#include <ranges>
//using namespace rg = std::ranges;

#include "range-v3/range/v3/all.hpp"
namespace rg = ::ranges;
namespace vs = rg::views;

#define TRACE_PRETTY
#include "trace.hpp"

using namespace std;


#ifdef __cpp_lib_ranges
template<std::ranges::input_range V>
    requires std::ranges::view<V>
//        && std::ranges::input_range<std::ranges::range_reference_t<V>>
std::ostream& operator<<(std::ostream& os, V view) {
    os << '[';
    auto b = view.begin();
    if (b != view.end()) {
        os << *b;
        while (++b != view.end()) os << ',' << *b;
    }
    os << ']';
    return os;
};
#endif

//template <class Printable>
//ostream& operator<<(ostream& os, const Printable& p) {
//    if constexpr (is_convertible)
//    return os << p;
//}

struct S;
ostream& operator<<(ostream& os, const S& p);

struct S {
    enum {None = -1};
    S(){ TraceX(*this); }
    S(int i) : id(i) { TraceX(*this); }
    S(const S& s) : id(s.id) { TraceX(*this); }
    S(S&& s) : id(s.id) { s.id = None; TraceX(*this); }
    S& operator=(const S& s) { id = s.id; TraceX(*this); return *this; }
    S& operator=(S&& s) { id = s.id; s.id = None; TraceX(*this); return *this; }
    ~S() { TraceX(id); }
    int id = None;
};

ostream& operator<<(ostream& os, const S& p) {
    return os << p.id;
}

template <typename T>
void test() {
    boost::circular_buffer<T> buf(8);
//    buf.resize(8);
    TraceX(buf.size(), buf.capacity());

    log_trace << "init with push_back:";
    for (int k=0; k<8; ++k) buf.push_back(k);
    TraceX(buf | vs::all);

    log_trace << "resize(0):";
    buf.resize(0);
    TraceX(buf | vs::all);

    log_trace << "refill with push_back:";
    for (int k=0; k<8; ++k) buf.push_back(k);
    TraceX(buf | vs::all);

    log_trace << "refill with generate:";
    std::generate(buf.begin(), buf.end(), [k=0]() mutable -> T { return k++;});
    TraceX(buf | vs::all);

    log_trace << "Continue with push_back";
    for (int k=buf.size(), e=k+4; k<e; ++k)  buf.push_back(k);
    TraceX(buf | vs::all);
    log_trace << "Continue with push_back";
    for (int k=buf.size(), e=k+4; k<e; ++k)  buf.push_back(k);
    TraceX(buf | vs::all);
//    auto v = buf | vs::filter([](auto k) { return (k & 1) == 0; });
//    auto v = vs::iota(0,8) | vs::filter([](auto k) { return (k & 1) == 0; });
    TraceX(buf | vs::stride(2));
}

struct U { uint8_t v; };
void test2() {
	using T = uint8_t;
	boost::circular_buffer<T> buf(8);
	auto offset = [](T x, T a) -> int {
		if (T(x - a) < T(a - x)) { // x > a
			return (int)(int8_t)(x - a);
		} else {// x < a
			return (int)(int8_t)(x - a);
		}
	};
	TraceX(offset(8, 5));
	TraceX(offset(1, 255));
	TraceX(offset(5, 8));
	TraceX(offset(255, 1));
	TraceX(offset(0, 128));
	TraceX(offset(128, 0));

	[[maybe_unused]] auto insert = [&](T x) {
//		T last = buf.back();
	};
	T a = 1;
	T b = 255;
	TraceX( (int)(T)(a - b));
}

void testMap() {
	struct T {
		explicit T(int x) : v(x) {}
		int v;
	};
	using TP = unique_ptr<T>;
	using TMap = multimap<int, TP>;
	TMap tmap;
	for (int k : vs::iota(8, 16)) tmap.insert({k, make_unique<T>(k)});
	TP t120{new T(120)};
	tmap.insert({12, std::move(t120)});
	for (auto& it : tmap) cout << it.first << " : " << it.second->v << '\n';
}

int main() {
//    test<S>();
	test2();
	testMap();
    return 0;
}