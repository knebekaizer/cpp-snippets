//
// Created by vdi on 20.08.24.
//


extern int f(int& n);
extern int f(const int& n);

template <typename T> int g1(const T n) { return f(n); }
template <typename T> int g2(const T n) { return f(n); }

void test2() {
    g1(0);
    g2(0);
}

