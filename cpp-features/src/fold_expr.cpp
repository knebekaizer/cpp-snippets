#include "trace.hpp"

#include <utility>
#include <functional>
#include <cstdint>
#include <iostream>
#include <array>
#include <cassert>

using  namespace std;

constexpr array<const char*, 4> tags{"none", "one", "two", "three"};

namespace {
static constexpr struct {
    template <typename T>
    auto operator()(T && n) const
    {
        return n*n;
    }
    template <typename T, typename... S>
    auto operator()(T && n, S && ... s) const
    {
        return (n * n) + (*this)(::std::forward<S>(s)...);
    }
} sumsquares;
}


template<class F, class A0>
auto fold(F&& f, A0&& a0) {
    f(std::forward<A0>(a0));
    return std::forward<A0>(a0);
}

template<class F, class A0, class...As>
auto fold(F&& f, A0&&a0, As&&...as) {
    f(std::forward<A0>(a0));
    return fold(f, std::forward<As>(as)...);
}

auto sum_squares=[](auto&&...args) {
    return fold(std::plus<>{}, (args * args)... );
};

int print(int x) { cout << x << endl; return 0; }

auto printAll=[](auto&&...args) {
    return fold(print, (args * args)... );
};

using E = int;

constexpr int s2e_unfold(const char* str) {
    return -1;
}

template <class...As>
constexpr int s2e_unfold(const char* str, int&& i, As&&...as) {
    if (str == tags[i]) return i;
    return s2e_unfold(str, std::forward<As>(as)...);
}

template <typename E, typename T, T... index>
constexpr E s2e(const char* str, std::integer_sequence<T, index...> seq) {
    return s2e_unfold(str, index...);
}

template <typename E>
constexpr E s2e(const char* str) {
    return s2e<E>(str, std::make_integer_sequence<int, 4>{});
}

using EValue = int;
/// Tag suffix `_v` for Equivalence class: Value
constexpr EValue operator""_v (const char* str, std::size_t);

// postgen
constexpr EValue operator""_v(const char* str, std::size_t) {
    return s2e<EValue>(str);
}

template <int i> void foo() {
    TraceX(i);
}

int main() {
//    printAll(1,2,3);
//    auto found = s2e<int>("one");
//    TraceX(found);
//    TraceX((s2e<int>("two")));
//    TraceX((s2e<int>("blabla")));
    foo<"one"_v>();
    foo<"two"_v>();
}
