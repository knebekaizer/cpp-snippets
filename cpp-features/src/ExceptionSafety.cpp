//! @file Exception safety example>

#include "../include/trace.h"
#include <vector>

using namespace std;

struct Foo {
private:
	static int _e;
public:
	Foo() : ex(Foo::_e++) {};
	Foo(int _ex) : ex(_ex) {}
	Foo(const Foo& other) : ex(other.ex) { log_trace << "Foo copy ctor"; if (ex < 0) throw runtime_error("copy ctor throws"); }
	Foo(Foo&& other) : ex(other.ex) { log_trace << "Foo move ctor"; other.ex = 0; }
	Foo& operator=(const Foo&) = default;
	Foo& operator=(Foo&&) = default;
	~Foo() { log_trace << "~Foo dtor " << ex ; }

	int ex;
};

int Foo::_e = 0;

void test_reserve() {
	using VFoo = vector<Foo>;
	VFoo v(4);
	TraceX(v.capacity(), v.size());
//	v.reserve(8);
	v.push_back(Foo(-1)); // move ctor, does not throw
	TraceX(v.capacity(), v.size());
	try {
		v.reserve(16);
	}
	catch (exception& e) {
		log_trace << "Exception caught: " << e.what();
	}
	TraceX(v.capacity(), v.size());
}

int main() {
	test_reserve();
}
