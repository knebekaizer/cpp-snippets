//
// Created by Vladimir Ivanov on 01.05.2021.
//

#include <sstream>
#include <algorithm>
#include <iostream>
#include <span>

#include "range/v3/all.hpp"

#include "scoped.hpp"

#include "trace.hpp"

using namespace std;
using namespace ranges;


void ttt() {
    auto letters = views::iota('a','g');
    std::cout << letters << '\n';
    // prints: {a,b,c,d,e,f,g}
    std::cout << (letters | views::slice(2,5)) << '\n';
    // prints: {c,d,e}
    auto res = letters[{2, 5}];
    vector<char> v(res.begin(), res.end());
    for (auto& x : v) cout << x << ", "; cout << endl;
}

template <typename T>
ostream& operator<<(ostream& os, const vector<T>& v) {
    if (!v.empty()) {
        os << "{" << v[0];
        std::for_each(1 + v.begin(), v.end(), [&os](const auto& x){ os << ", " << x; });
        os << "}";
    }
    return os;
}

//template <ranges::input_range T>
//ostream& operator<<(ostream& os, const T& v) {
//    if (!v.empty()) {
//        os << "{" << v[0];
//        std::for_each(1 + v.begin(), v.end(), [&os](const auto& x){ os << ", " << x; });
//        os << "}";
//    }
//    return os;
//}

template <size_t N>
ostream& operator<<(ostream& os, const array<char, N>& v) {
    return os << &v[0];
}

template <typename T, size_t N>
ostream& operator<<(ostream& os, const array<T, N>& v) {
    if (!v.empty()) {
        os << "{" << v[0];
        std::for_each(1 + v.begin(), v.end(), [&os](const auto& x){ os << ", " << x; });
        os << "}";
    }
    return os;
}

vector<string> formatStr(int n) {
//    std::vector<char[8]> out(n+1);

    using C = array<char, 8>; //char C[8];
    array<C, 17> buf = {{{'\0'}}};
    auto fmt = views::iota(1, n+1)
            | views::transform([](auto x) { return string("%") + to_string(x) + "llx"; });
//            | views::for_each([](auto x) { return string("%") + to_string(x) + "llx"; });

//    std::for_each(buf.begin() + 1, buf.end(), [k=0](auto& p) mutable { views::copy( ("%"s + to_string(k) + "llx"s),  )})
    string s = fmt[0];
    std::copy(s.begin(), s.end(), buf[1].begin());
    std::copy_n(fmt[0].begin(), fmt[0].size(), buf[1].begin());
//    auto p = buf[1]; for (auto c : fmt[0]) *p++ = c;
    TraceX(&buf[1][0]);

//    int k = 0;
//    for (auto const& s: fmt) std::copy(s.begin(), s.end(), &buf[++k][0]);

//    std::for_each(fmt.begin(), fmt.end(), [p=buf.begin()](const auto& s) mutable { std::copy_n(s.begin(), s.size(), (++p)->begin()); });
//    for (auto const& x : buf) TraceX(&x[0]);
//    TraceX(buf);

    return {fmt.begin(), fmt.end()};
}


using namespace std::literals;
auto test_gen() {
    constexpr int n = 16;
    auto gen = views::iota(1, n+1)
                | views::transform([](auto x) { return "%"s + to_string(x) + "llx"; });
    TraceX(gen[n]);

    static char buf[17][8] = {{0}};
    ranges::for_each(gen, [p = buf](const auto& x) mutable { ranges::copy(x, (*++p)); } );
//    ranges::for_each(gen, [](const auto& x)  { TraceX(x); });
    TraceX(buf[16]);

    array<char, 8> a = {"qwerty"};
    TraceX(a);

    static std::span ret(buf);
    return ret;
}

auto test_gen2() {
    auto gen_fn = []() {
        constexpr int n = 16;
        auto gen = views::iota(1, n + 1)
                | views::transform([](auto x) { return "%"s + to_string(x) + "llx"; });
        TraceX("lambda> ", gen[n]);

        static char buf[17][8] = {{0}};
//        ranges::for_each(gen, [p = buf](const auto& x) mutable { ranges::copy(x, (*++p)); });


        ranges::for_each(
                buf | views::drop(1),
                [k=0](auto& p) mutable { ranges::copy( ("%"s + to_string(++k) + "llx"), p); });

        for (int k = 0; auto& p : buf | views::drop(1))
            ranges::copy( ("%"s + to_string(++k) + "llx"), p);

        array<char, 8> a = {"qwerty"};
        TraceX(a);
        return &buf;
    };

    static std::span ret(*gen_fn());
    return ret;
}

//#include <range/v3/range/conversion.hpp>
//#include <range/v3/view/for_each.hpp>
//#include <range/v3/view/iota.hpp>
//#include <range/v3/view/repeat_n.hpp>

void ex3()
{
    using namespace ranges;
    auto vi = views::for_each(views::ints(1, 6),
                              [](int i) { return yield_from(views::repeat_n(i, i)); }) |
              to<std::vector>();
    // prints: [1,2,2,3,3,3,4,4,4,4,5,5,5,5,5]
    cout << views::all(vi) << '\n';

    auto jj = views::iota(21, 24);
    cout << jj << endl;
}

//namespace rng = ranges;
//
//template <rng::range R>
//constexpr auto to_vector(R&& r) {
//    using elem_t = std::decay_t<rng::range_value_t<R>>;
//    return std::vector<elem_t>{r.begin(), r.end()};
//}
//void t2() {
//    auto letters = views::iota('a','g');
//    vector<char> v = to_vector(letters);
//}



void ex4() {

    auto print = [](auto const& view) {
        for (std::cout << "{"; const auto& element : view)
            std::cout << element;
        std::cout << "} ";
    };
    print(views::empty<string>); cout << endl;

    static constexpr auto input = "Niebloid comes from Eric Niebler's name. In simple words, they are function objects"
                                  " that disable ADL (argument-dependent lookup)";

//    auto words = input | views::split(' ');
//    cout << words[0];
//    for (auto w : input | views::split(' ')) cout << string(w.data());
    istringstream   text(input);
    auto words = istream_view<string>(text);
//    cout << words << endl;

    auto lens = words | views::transform(&string::size);
    TraceX(lens);
    TraceX(lens);

    auto maxLen = ranges::max(words | views::transform(&string::size));
    TraceX(maxLen);
    TraceX(ranges::max(lens));
//    ranges::less trivial = {};
//    auto maxLen = ranges::max_element(words | view::transform(&string::size), [](auto a, auto b) { return a<b;});
//    print(words);
}

int main() {
    auto _ = Scoped {
        [](){ log_trace_(main) << "Starting..."; },
        [](){ log_trace_(main) << "Done."; }
    };

    ttt();

    auto fmt = formatStr(16);
    log_trace << fmt;

    for (auto k : {0,1,2,3}) {  test_gen2(); TraceX(k); }

    for (const auto& x : test_gen2()) cout << x << ", "; cout << endl;

    ex3();
    ex4();
    return 0;
}