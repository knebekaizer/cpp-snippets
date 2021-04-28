#include <cstdint>

class A {
public:
    void foo(int64_t n, int k = 0);
private:
    void foo(const char* s);
};

class B {
public:
    void foo(double x);
};

class C : public B {
private:
    void foo(int x, int y);
};

void test_accessibility() {
    A().foo(0L);

    C().foo(0.0);
}
