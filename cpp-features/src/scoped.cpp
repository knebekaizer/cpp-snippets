#include <exception>
#include <cassert>
#include "trace.h"

/// Scope-based init / teardown
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


#pragma mark ----

struct Dummy {
	void lock() { log_trace << "Dummy instance " << this << " locked"; }
	void unlock() { log_trace << "Dummy instance " << this << " unlocked"; }
};

template <typename RunMe>
class Sync2 {
	const RunMe run_;
	static Dummy dummy_;
public:
	explicit Sync2(RunMe&& run) : run_(run) {}
	void operator()() {
		Scoped guard {
			[]() { dummy_.lock(); },
			[]() { dummy_.unlock(); }
		};
		run_();
	}
};

class SyncAt {
//	const RunMe run_;
public:
	explicit SyncAt() { TraceF; }
	~SyncAt() { TraceF; }
	template <typename RunMe>
	void operator()(RunMe&& block) { block(); }
};

/// ^^^^^^^^^^^^^^^^^^^^^

using namespace std;

bool lock(int& x) { TraceF; return ++x; }
void unlock(int& x) { TraceF; --x; }
//void unlock0() { TraceF; }

void foo() {
	log_trace << "Gonna to throw an exception...";
	throw runtime_error("Test exception");
}

//void lock() { log_trace << __func__;  }

void test(int& x) {

	log_trace << "Running " << __func__ << " at " << __FILE__;
	auto guard = Scoped {
		[&]() { lock(x); },
		[&]() { unlock(x); }
	};

	Scoped nested { []() { log_trace << "Tear down: this should happen before unlock!"; }};

	TraceX(x);
	foo();        // this will throw eexception
	unlock(x);    // so this is unreachable
	TraceX(x);
	// Expecting 2 scoped dtors
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

	SyncAt() ( []() {
		log_trace << "This is a block!";
	});
}