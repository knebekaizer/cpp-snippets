//
// Created by vdi on 20.08.24.
//
#include "trace.hpp"


int f(int& n) { log_trace << __PRETTY_FUNCTION__;  return 1; }
int f(const int& n) { log_trace << __PRETTY_FUNCTION__; return 2; }


/// Templates g1(const T) and g2(const T n) are defined in other TU

// Clang-Tidy: Parameter 'n' is const-qualified in the function declaration; const-qualification of parameters only has an effect in function definitions
template <typename T> int g1(const T n); // { return f(n); }
template <typename T> int g1(T n) { return f(n); } // this compiles when g(const int n) not implemented / not instantiated
// Only g1(T) is instantiated and calls f(int& n)

template <typename T> int g2(const T n); // { return f(n); }
template <typename T> int g2(T n) { return f(n); } // this compiles when g(const int n) not implemented / not instantiated
extern template int g2<int>(const int n); // ???
// Now g2(const T n) instantiated and calls f(const int& n)

void test() {
    g1(0);
    g2(0);
}

/// The same function but it is declared where `template <typename T> int g(const T n)` is visible and instantiated.

void test2(); // { g(0); }

int main() {
    test();
    test2();
}