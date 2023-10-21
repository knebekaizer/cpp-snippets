#include <exception>
#include <cassert>
#include <functional>
#include "trace.h"


/// Scope-based init / teardown
class Scoped {
	static constexpr void nop() {}
public:
	using Function = std::function<void()>;

	explicit Scoped(Function teardown) : atExit_(std::move(teardown)) {}
	Scoped(Function init, Function teardown) : Scoped(std::move(teardown)) { init(); }
	~Scoped() { atExit_(); }

	Scoped(const Scoped&) = delete;
	Scoped(Scoped&& other) : atExit_(std::move(other.atExit_)) { other.atExit_ = nop; };
	Scoped& operator=(const Scoped& other) = delete;
	Scoped& operator=(Scoped&&) = delete;

private:
	Function atExit_ = nop;
};


template <class Mutex = std::mutex>
class Synchronized : private std::unique_lock<Mutex> {
//	std::unique_lock<Mutex> mtx_;
	using Base = std::unique_lock<Mutex>;
public:
	explicit Synchronized(Mutex& m) : Base(m) {}

};

using namespace std;
void test() {
	std::mutex mtx;
	struct Sync {
		std::unique_lock<std::mutex> mtx_;
	};

}
struct Dummy {
	void lock() { log_trace << "Dummy instance " << this << " locked"; }
	void unlock() { log_trace << "Dummy instance " << this << " unlocked"; }
};


using namespace std;

bool lock(int& x) { TraceF; return ++x; }
void unlock(int& x) { TraceF; --x; }
//void unlock0() { TraceF; }

void foo() {
	log_trace << "Gonna to throw an exception...";
	throw runtime_error("Test exception");
}

//void lock() { log_trace << __func__;	}

void test(int& x) {
	log_trace << "Running " << __func__ << " at " << __FILE__;
	auto guard = Scoped {
		[&]() { lock(x); },
		[&]() { unlock(x); }
	};

	Scoped nested { []() { log_trace << "Tear down: this should happen before unlock!"; }};

	TraceX(x);
	foo();		  // this will throw eexception
	unlock(x);	  // so this is unreachable
	TraceX(x);
	// Expecting 2 scoped dtors
}

void test() {
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

int main() {
	test();
	return 0;
}