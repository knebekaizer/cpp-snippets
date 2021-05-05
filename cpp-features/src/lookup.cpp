#include <cstdint>

class A {
public:
	void foo(int64_t n, int k = 0) {};
private:
	void foo(const char* s) {};
};

class B {
public:
	void foo(double x) {};
};

class C : public B {
public:
private:
	void foo(int x, int y) {};
};

void test_accessibility() {
//	  A().foo(0); // error: call to member function 'foo' is ambiguous
	B().foo(0);

	C().B::foo(0.0);	// OK
//	  C().foo(0.0);		// error: 'foo' is a private member of 'C' ???	error: too few arguments to function call
}

int main() {
	// test it is compilable only
}