
#include "boost/circular_buffer.hpp"
#include <algorithm>

#include <ranges>
//using namespace rg = std::ranges;

#include "range-v3/range/v3/all.hpp"
namespace rg = ::ranges;
namespace vs = rg::views;

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

void test() {
    boost::circular_buffer<int> buf(16);
    buf.resize(16);
    TraceX(buf.size());
    std::generate(buf.begin(), buf.end(), [k=0]() mutable { return k++;});
    TraceX(buf | vs::all);
    for (int k=16; k<32; ++k)  buf.push_back(k);
    TraceX(buf | vs::all);
    auto v = buf | vs::filter([](auto k) { return (k & 1) == 0; });
//    auto v = vs::iota(0,8) | vs::filter([](auto k) { return (k & 1) == 0; });
    TraceX(v);
}

int main() {
    test();
    return 0;
}