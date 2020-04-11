#include <iostream>
#include "trace.h"
#include "features.h"

using namespace std;

void* myMalloc(size_t size) {
	void* p = ::malloc(size);
	log_trace << "size: " << size << "; p = " << p;
	return p;
}

void myFree(void* p) {
	log_trace << p;
	::free(p);
}

struct Base {
	void *operator new(size_t size) {
		log_trace << "Custom new operator with size: " << size;
		auto p = myMalloc(size);
		return p;
	}

	void operator delete(void* p) {
		log_trace << "Custom delete operator";
		myFree(p);
	}
};

#include <memory>
#include <array>
#include <vector>
#include <numeric>
using namespace std;

class A : public Base {
public:
	A(int x) { TraceF;
		generate(v.begin(), v.end(), [k = 0]() mutable { return k++; });
	}
	~A() { TraceF;
		for (auto k : {0, 12,256})  log_trace << "v[" << k << "] = " << v[k];
	}
	array<int, 1024> v;
};

void some_function()
{
    vector<int> v(10);
    std::iota(v.begin(), v.end(), 0);
    generate(v.begin(), v.end(), [k = 21]() mutable { return k++; });

    for (auto x : v) { cout << x << " "; }; cout << endl;
}

int main() {
	log_info << "Start";

	auto a = make_unique<A>(42);

	some_function();
	return 0;
}
