#include <exception>
#include "trace.h"


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



using namespace std;

bool lock(int& x) { return (x = 1); }
void unlock(int& x) { x = 0; }

void foo() {
	throw runtime_error("Test exception");
}

void test(int& x) {
	Auto(unlock(x));
	lock(x);
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
	} catch (exception& e) {
		log_info << e.what();
	}
	TraceX(x);
}