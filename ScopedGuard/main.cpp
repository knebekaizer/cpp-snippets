#include <exception>
#include "trace.h"

//! C++11
template <class Lambda> class AtScopeExit {
  Lambda& m_lambda;
public:
  AtScopeExit(Lambda& action) : m_lambda(action) {}
  ~AtScopeExit() { m_lambda(); }
};

#define TOKEN_PASTEx(x, y) x ## y
#define TOKEN_PASTE(x, y) TOKEN_PASTEx(x, y)

#define Auto_INTERNAL1(lname, aname, ...) \
    auto lname = [&]() { __VA_ARGS__; }; \
    AtScopeExit<decltype(lname)> aname(lname);

#define Auto_INTERNAL2(ctr, ...) \
    Auto_INTERNAL1(TOKEN_PASTE(Auto_func_, ctr), \
                   TOKEN_PASTE(Auto_instance_, ctr), __VA_ARGS__)

#define Auto(...) Auto_INTERNAL2(__COUNTER__, __VA_ARGS__)

//! C++17 ???

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

bool lock(int& x) { TraceF; return (++x); }
void unlock(int& x) { TraceF; --x; }
void unlock0() { TraceF; }

void foo() {
	TraceF;
	throw runtime_error("Test exception");
}

void test(int& x) {
//	Auto(unlock(x)); // ok
//	lock(x);

	Scoped guard {
		[&]() { lock(x); },
		[&]() { unlock(x); }
	};

	Scoped atExit { []() { log_trace << "tear down!"; }};

	TraceX(x);
	foo();
	unlock(x);
	TraceX(x);
}

int main() {
	log_info << "Start";
	auto x = 0;
	TraceX(x);

	try {
		test(x);
		log_warn << "This should be unavailable code!";
	} catch (exception& e) {
		log_info << e.what();
	}
	TraceX(x);
}