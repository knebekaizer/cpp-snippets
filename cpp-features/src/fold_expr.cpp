#include "trace.hpp"

#include <utility>
#include <functional>
#include <cstdint>
#include <iostream>

using  namespace std;

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

template <typename E, typename T, T... index> constexpr E s2e(const char* str, std::integer_sequence<T, index...> seq) {
    constexpr T invalid = static_cast<T>(-1);
    T found = invalid;
    //    return (str == Tags<EValue>[1] ? EValue::one : (str == Tags<EValue>[2] ? EValue::two : EValue::none));
    //    [[maybe_unused]] constexpr auto isValid = [invalid](T x) { return x != invalid; };
//    (isValid(found = (std::get<index>(Tags<E>) == str) ? index : found) || ...);
    printAll(index...);
    return static_cast<E>(found);
}

template <typename E> constexpr E s2e(const char* str) {
    return s2e<E>(str, std::make_integer_sequence<int, 4>{});
}


int main() {
    printAll(1,2,3);
    s2e<int>("one");
}
