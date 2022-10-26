
#include "boost/circular_buffer.hpp"
#include <algorithm>
#include <type_traits>

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

int main() {
    test<S>();
    return 0;
}