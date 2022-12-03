//
// Created by Vladimir on 10.10.2022.
//

typedef decltype(nullptr) nullptr_t;

#include <version>
#include <cstddef>
#include <initializer_list>
#include <cassert>
//#include <algorithm>
//#include <iostream>
#include <vector>
#include <span>
#include <algorithm>

#include "scoped.hpp"

#include "trace.hpp"

#ifdef NO_STD_RANGES
#include "range/v3/all.hpp"
namespace rg = ::ranges;
namespace vs = ::ranges::views;
#else
#if __has_include(<ranges>)
//#undef _LIBCPP_HAS_NO_INCOMPLETE_RANGES
#include <ranges>
namespace rg = std::ranges;
namespace vs = std::ranges::views;
#endif
#endif // NO_STD_RANGES

#ifndef __cpp_lib_ranges
#pragma message("std::ranges not available")
#endif

using namespace std;
using namespace std::literals;

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

template <bool longMask = true>
class Bitmap {
public:
    Bitmap() = default;
    Bitmap(std::initializer_list<size_t> ls) { set(ls); }
    void set(size_t k) {
        assert(k < 8 * N);
        buf_[k >> 3] |= (1 << (k & 7));
    }
    void set(std::initializer_list<size_t> ls) {
        for (auto x : ls) set(x);
    }

    bool get(size_t k) const {
        assert(k < 8 * N);
        return buf_[k >> 3] & (1 << (k & 7));
    }
    bool operator[](size_t k) const { return get(k); }
    auto indices() const {
        return vs::iota(0,longMask ? 48 : 16)
            | vs::filter([&](auto k) { return 1 & (buf_[k >> 3] >> (k & 7)); });
    }

//    auto indices() const {
//        return vs::iota(0,longMask ? 48 : 16) | vs::filter( [this](auto k){ return this->get(k);} );
//    }
//    auto indices2() const { return buf_ | vs::for_each([base = -8](auto k) mutable { base += 8;
//                   return vs::ints(base, base + 8) | vs::filter( [k](auto bit){ return k & (1 << (bit & 7));}); }); }
#ifdef RANGES_V3_RANGE_HPP
    auto indices2() const { auto base = -8; return buf_ | vs::for_each([base](auto k) mutable { base += 8;
                   return vs::iota(base, base + 8) | vs::filter( [k](auto bit){ return k & (1 << (bit & 7));}); }); }
#else
    auto indices2() const { return "Not implemented"s; }

#endif
    static constexpr size_t N = longMask ? 6 : 2;
    uint8_t buf_[N] = {0};
};

#ifdef RANGES_V3_RANGE_HPP
template <bool longMask> auto indices(uint8_t buf[longMask ? 6 : 2]) {
    return ::ranges::make_span(buf, longMask ? 6 : 2) | vs::for_each([base = -8](auto k) mutable {
               base += 8;
               return vs::ints(base, base + 8) | vs::filter([k](auto bit) { return k & (1 << (bit & 7)); });
           });
}
#endif
// FIXME
auto indices(uint8_t* buf, bool longMask) {
    // TODO Use std::numeric_limits<decltype(*buf)>::digits for the number of bits in the type
    auto scanBits = [](uint8_t i) {
        return vs::iota(0, 8) | vs::filter([i](auto bit) { return 1 & (i >> bit); });
    };
    return std::span(buf, longMask ? 6 : 2) | vs::transform([scanBits, base = -8](auto k) mutable {
               base += 8;
               return scanBits(k) | vs::transform([base](auto bit){ return base + bit; });
           });
}


void test_staticLambda() {
    auto lam = [base = 0]() mutable {
        base += 10;
        return vs::iota(base, base + 8);
    };
    TraceX(lam());
    TraceX(lam());

    auto fun = [] {
        auto lam = [base = 0]() mutable {
            base += 10;
            return vs::iota(base, base + 8);
        };
        return lam();
    };
    TraceX(fun());
    TraceX(fun());
}

//#ifdef RANGES_V3_RANGE_HPP
void bm_test() {
    TraceX(sizeof(Bitmap<1>));
    //    Bitmap bm;
    //    bm.set({0,3,6,8,15,23,37,35,40,46,47});
    Bitmap bm{0,3,6,8,15,23,37,35,40,46,47};
    
    for (auto k = 0u; k < bm.N * 8; ++k)
        if (bm[k]) cout << k << ',';
    cout << endl;
    TraceX(bm.indices());
    TraceX(bm.indices2());

    uint8_t buf[2] = {0b10001001, 0b110};
    auto bm2 = (Bitmap<0>*)buf;
    TraceX(bm2->indices2());

//    TraceX(indices<0>(buf));
//    TraceX(indices<0>(bm.buf_));
//    TraceX(indices<1>(bm.buf_));
    TraceX(indices(bm.buf_, false));
    TraceX(indices(bm.buf_, true));

    auto base = 100;
    auto listSN_0 = bm.indices() | vs::transform([base](auto k){ return base + k; });
    TraceX(listSN_0);
    TraceX(listSN_0);
//    auto listSN_1 = [](auto buf, bool longMask, int base) {
//        return indices(buf, longMask) | vs::transform([base](auto k){ return base + k; });
//    };
////    auto listSN_1 = bm.indices2() | vs::transform([base](auto k){ return base + k; });
//    cout << listSN_1(bm.buf_, true, 100);
//    TraceX(listSN_1(bm.buf_, true, 100));
//    TraceX(listSN_1(bm.buf_, true, 100));
}
//#endif


//template< ranges::input_range V, ranges::forward_range Pattern >
//    requires ranges::view<V> && ranges::input_range<ranges::range_reference_t<V>> &&
//    ranges::view<Pattern> &&
///* range_reference_t<V> and Pattern have compatible elements (see below) */
//class join_with_view : ranges::view_interface<join_with_view<V, Pattern>>


void print(auto comment, auto const& seq, char term = ' ') {
    for (std::cout << comment << '\n'; auto const& elem : seq)
        std::cout << elem << term;
    std::cout << '\n';
}

struct Particle {
    std::string name; double mass; // MeV
    template<class Os> friend
        Os& operator<< (Os& os, Particle const& p) {
        return os << std::left << std::setw(8) << p.name << " : " << p.mass << ' ';
    }
};

void test_sort()
{
   std::array s {5, 7, 4, 2, 8, 6, 1, 9, 0, 3};

   namespace ranges = std::ranges;

   ranges::sort(s);
   print("Sort using the default operator<", s);

   ranges::sort(s, ranges::greater());
   print("Sort using a standard library compare function object", s);

   struct {
       bool operator()(int a, int b) const { return a < b; }
   } customLess;
   ranges::sort(s.begin(), s.end(), customLess);
   print("Sort using a custom function object", s);

   ranges::sort(s, [](int a, int b) { return a > b; });
   print("Sort using a lambda expression", s);

   Particle particles[] {
       {"Electron", 0.511}, {"Muon", 105.66}, {"Tau", 1776.86},
       {"Positron", 0.511}, {"Proton", 938.27}, {"Neutron", 939.57},
   };
   ranges::sort(particles, {}, &Particle::name);
   print("\nSort by name using a projection", particles, '\n');
   ranges::sort(particles, {}, &Particle::mass);
   print("Sort by mass using a projection", particles, '\n');
}

namespace detail {
// Type acts as a tag to find the correct operator| overload
template <typename C>
struct to_helper {
};

// This actually does the work
template <typename Container, rg::range R>
    requires std::convertible_to<rg::range_value_t<R>, typename Container::value_type>
Container operator|(R&& r, to_helper<Container>) {
    return Container{r.begin(), r.end()};
}
}

// Couldn't find an concept for container, however a
// container is a range, but not a view.
template <rg::range Container>
    requires (!rg::view<Container>)
auto to() {
    return detail::to_helper<Container>{};
}


void test_std() {
//    test_sort();
    std::array s [[maybe_unused]]  {5, 7, 4, 2, 8, 6, 1, 9, 0, 3};
//    auto s1 = rg::ref_view(vs::iota(0, 8));
//  auto s1 = rg::owning_view(vs::iota(0, 8));
    auto v = vs::iota(0, 8) | to<vector<int>>();
    TraceX(rg::ref_view(v));
    TraceX(v | vs::all);
    rg::sort(v, rg::greater());
    TraceX(v | vs::all);
//    auto v3 [[maybe_unused]] = rg::sort(vs::iota(0, 8) | to<vector<int>>(), rg::greater());
//    for (auto x : v3) cout << x << ","; cout << endl;
//    TraceX(rg::ref_view(rg::sort(v, rg::greater())));
//    auto s2 = rg::sort(s1);
//    auto ov [[maybe_unused]] = rg::owning_view(vs::iota(0, 8));  // not in g++10
//    rg::sort(ov, rg::greater());
//    TraceX(ov);
//    rg::sort(rg::ref_view(vs::iota(0, 8)), rg::greater());
//    TraceX( rg::sort(rg::ref_view(vs::iota(0, 8)), rg::greater()) );
//    TraceX( rg::sort(rg::owning_view(s), rg::greater()) );
//    TraceX( rg::sort(rg::owning_view(vs::iota(0, 8)), rg::greater()) );

    Bitmap bm{0,3,6,8,15,23,37,35,40,46,47};
    auto bin = vs::iota(0, 48) | vs::transform([buf = bm.buf_](auto k) {
                  return (buf[k >> 3] >> (k & 7)) & 1; });
    TraceX(bin);
//    TraceX(bin | vs::all);

    auto v1 = vs::iota(0, 48) | vs::filter([buf = bm.buf_](auto k) {
                  return 1 & (buf[k >> 3] >> (k & 7)); });
    TraceX(v1);

    auto std_indices = [](auto buf) {
        return buf | vs::transform([base = -8](uint8_t k) mutable {
                   base += 8;
                   return vs::iota(0, 8)
                       | vs::filter([k](auto bit) { return (k >> bit) & 1; })
                       | vs::transform([base](auto bit) { return base + bit; });
               }) | vs::join;
    };

    cout << std_indices(std::span(bm.buf_, 2));
    TraceX(std_indices(std::span(bm.buf_, 2)));
    TraceX(std_indices(std::span(bm.buf_, 2)));
    TraceX(std_indices(std::span(bm.buf_, 6)));
    TraceX(std_indices(std::span(bm.buf_, 6)));

//    bm.buf_ | vs::transform([](auto k) { return to_string(k); });
}

void test_print() {
    std::cout << vs::iota(0,8) << std::endl;
    std::cout << vs::iota(0,0) << std::endl;
}

int main() {
    auto _ = Scoped {
        [](){ log_trace_(main) << "Starting..."; },
        [](){ log_trace_(main) << "Done."; }
    };

    bm_test();
    test_staticLambda();
    test_std();
    test_print();
    return 0;
}
