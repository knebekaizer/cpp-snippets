#include <exception>
#include <cassert>
#include "trace.h"


template <class Exit, class Enter = Exit > class Scoped {
	const Exit atExit_;
	Scoped(const Scoped&) = delete;
	Scoped(Scoped&&) = delete;
	Scoped& operator=(const Scoped& other) = delete;
	Scoped& operator=(Scoped&&) = delete;
public:
	explicit Scoped(const Exit& teardown) : atExit_(teardown) {}
	Scoped(const Enter& init, const Exit& teardown) : Scoped(teardown) { init(); }
	~Scoped() { atExit_(); }
};

template <class F2> Scoped(F2) -> Scoped<F2, F2>;
template <class F1, class F2> Scoped(F1, F2) -> Scoped<F2, F1>;

/// ^^^^^^^^^^^^^^^^^^^^^

using namespace std;

bool lock(int& x) { TraceF; return ++x; }
void unlock(int& x) { TraceF; --x; }
void unlock0() { TraceF; }

void foo() {
	log_trace << "Gonna to throw an exception...";
	throw runtime_error("Test exception");
}

void lock() { log_trace << __func__;  }

void test(int& x) {

	auto guard = Scoped {
		[&]() { lock(x); },
		[&]() { unlock(x); }
	};

	Scoped nested { []() { log_trace << "Tear down: this should happen before unlock!"; }};

	TraceX(x);
	foo();
	unlock(x);
	TraceX(x);
}

void scoped() {
	log_info << "Scoped lock test started";
	auto x = 0;
	TraceX(x);

	try {
		test(x);
		log_warn << "This line should never be reached!";
	} catch (exception& e) {
		log_info << "Caught: " << e.what();
	}
	TraceX(x);
	assert(x == 0);
}