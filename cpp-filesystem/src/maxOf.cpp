

#include <string>
#include <cassert>
#include <iostream>
#include <vector>

#include "trace.h"

using namespace std;

void test_max() {
    std::vector<int> v{ 3, 1, -14, 1, 5, 9 };
    std::vector<int>::iterator result;

    result = std::max_element(v.begin(), v.end());
    std::cout << "max element at: " << std::distance(v.begin(), result) << '\n';

    result = std::max_element(v.begin(), v.end(), [](auto a, auto b) { return abs(a) < abs(b); });
    std::cout << "max element (absolute) at: " << std::distance(v.begin(), result) << '\n';
}

///  @return a **copy** of max element
template <typename T, typename Op>
auto maxElemOf(const T t, Op op) {
    return *std::max_element(begin(t), end(t), [op](auto a, auto b) { return op(a) < op(b); });
}

template <typename T, typename Op>
auto maxOf(const T t, Op op) {
//    auto elem = std::max_element(begin(t), end(t), [op](auto a, auto b) { return op(a) < op(b); });
//    TraceX("Done.");
    return op(maxElemOf(t, op));
}

struct Vector : public vector<string> {
    template <typename T>
    Vector(T t) : vector<string>(t) { TraceF; }

    Vector(const Vector& v) : vector<string>(v) { TraceX("copy ctor"); }
    ~Vector() { TraceF; }
    using vector<string>::size;
};

void test_maxOf() {
    std::vector<int> v{ 3, 1, -14, 1, 5, 9 };
    auto res = maxOf(v, [](auto a) { return abs(a); });
    TraceX(res);

    TraceX(maxOf(vector<int>{42, 11, 9}, [](auto x) { return x % 7; }));
    TraceX(maxOf(vector<string>{"one", "two", "three"}, [](auto x) { return x.size(); }));
    vector<string> ilist = {"one", "two", "three"};
//    auto found = maxElemOf(vector<string>{"one", "two", "three"}, [](auto x) { return x.size(); });
    auto longest_word = maxElemOf(vector<string>{"one", "two", "three"}, mem_fn(&string::size));
    TraceX(longest_word);
}


int main() {
    test_max();
    test_maxOf();
    return 0;
}