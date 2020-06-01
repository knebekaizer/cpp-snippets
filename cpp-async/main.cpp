//#include <iostream>
//
//int main() {
//	std::cout << "Hello, World!" << std::endl;
//	return 0;
//}

#include <thread>
#include <future>
#include <iostream>
#include <chrono>
#include <vector>
#include <array>
#include <cassert>

#include <cstdlib>  // _Exit()    "stdlib.h"
#include <unistd.h>  // sleep()
#include <csignal>  // signal.h

#include <execinfo.h>
int backtrace(void **buffer, int size);
char **backtrace_symbols(void *const *buffer, int size);

#include "trace.h"

using namespace std;

volatile size_t value = 0;

void my_sig_handler(int signal) {
	TraceX(signal);
}

void set_abort_handler() {
	struct sigaction act = {my_sig_handler};
	sigaction(SIGABRT, &act, 0);
}


int print_bt(ostream& ost = cerr) {
	std::array<void*, 10> buf = {0};
    size_t size;

	// get void*'s for all entries on the stack
	size = backtrace(&buf[0], buf.size());

	// print out all the frames to stderr
	//	 fprintf(stderr, "Error: signal %d:\n", sig);
	auto bt = backtrace_symbols(&buf[0], size);

	ost << "Current stack:" << endl;
	for (int k=0; k<size; ++k) ost << bt[k] << endl;
	ost << "Done." << endl;

	return 1;
}

size_t loop(int limit) {
//  std::cout << "Started " << inc << " " << limit << std::endl;
  for (int i = 0; i < limit; ++i) {
    ++value;
  }
  terminate();
}

std::atomic<int> flag = 0;

void my_terminate() {
	if (flag++ == 0) {
		TraceF;
		print_bt();
		sleep(5);
	} else {
		std::this_thread::sleep_for(std::chrono::seconds(5));
		_Exit(1); // force exit
	}
	std::abort();
}

void my_terminate_2() {
	static int _ = print_bt();

	std::abort();
}

void setTerminateHandler();

namespace {
//static void my_handler();

/**
 * A trick similar to Meyers singleton is used to set the terminate handler and save
 * the old one in thread-safe manner.
 * The saved handler is not intended to be invoked from any caller
 * but my_handler() (even in this source file so anon namespace is not good enough)
 * and occasionally setTerminateHandler() also need to access queuedHandler() for setup.
 * To enforce this restriction it is impleneted as a private operator() with two friends only.
 */
class TerminateHandler {
	void operator()(bool install_only = false) const {
		auto init = []() {
			log_trace << "TerminateHandler init invoked";
			return std::set_terminate(my_handler);
		};
		static auto queuedHandler_ = init();
		if (install_only) return;
		queuedHandler_();
	}
	static void my_handler();
public:
	static void install();
} queuedHandler;

void TerminateHandler::my_handler() {
	TraceF;
	print_bt(cout);
	queuedHandler();
}

void TerminateHandler::install() {
	queuedHandler(true);
}

} // namespace

void setTerminateHandler() {
	TerminateHandler::install();
}


#pragma mark ----

namespace {
class TerminateHandler2 {
	/**
	 * 10 seconds is the timeout for concurrent (second) terminate to give a chance the first one to finish.
	 * After the timeout the abnormal _Exit will be invoked.
	 */
	static const int timeout = 5; ///< 10 seconds is the timeout for concurrent (second) terminate before abnormal _Exit will be invoked.

	typedef __attribute__((noreturn)) void (*QH)();
	QH queuedHandler_;



	TerminateHandler2()
		: queuedHandler_((QH)std::set_terminate(my_handler))  // set_terminate does not respect NORETURN so cast is needed
	{
		log_trace << "TerminateHandler init invoked";
	}

	static TerminateHandler2 *instance() {
		static TerminateHandler2 *singleton = new TerminateHandler2();
		return singleton;
	}

	// In fact, it's safe to call my_handler directly from outside: it will do the job and then invoke original handler,
	// even if it has not been initialized yet. So one may want to make it public and/or not the class member
	__attribute__((noreturn)) static void my_handler() {
		static std::atomic<int> flag = 0;  // Note: compiler does not use ___cxa_guard_* stuff for primitive type
		if (flag++ == 0) {     // atomic post-increment has an effect of test_and_set but simpler
			TraceF;
			print_bt(cout);
			sleep(10);  // for testing
			instance()->queuedHandler_();
		} else {
			sleep(timeout);
//			log_trace << "Go to _Exit now!";
			_Exit(EXIT_FAILURE); // force exit
		}
	}

	// Copy, move and assign would be safe, but not much useful, so let's delete all (rule of 5)
	TerminateHandler2(const TerminateHandler2 &) = delete;
	TerminateHandler2(TerminateHandler2 &&) = delete;
	TerminateHandler2 &operator=(const TerminateHandler2 &) = delete;
	TerminateHandler2 &operator=(TerminateHandler2 &&) = delete;
	// Dtor might be in use to restore original handler. However, consequent install
	// will not reconstruct handler anyway, so let's keep dtor deleted to avoid confusion.
	~TerminateHandler2() = delete;
public:
	/// First call will do the job, all consecuent will do nothing.
	static void install() {
		instance(); // only a side effect of warming up
	}
};
} // anon namespace

// Use one public funuction to limit access to the class declaration
void setTerminateHandler2() {
	TerminateHandler2::install();
}


int test_ConcurrentTerminate() {
    std::vector<std::future<size_t>> futures;

    for (size_t i = 0; i < 100; ++i) {
        futures.emplace_back(std::async(std::launch::async, loop
//        		[](size_t param){
//            std::this_thread::sleep_for(std::chrono::seconds(param));
//            return value;
//        }
        , 1000));
    }

    for (auto &future : futures) {
//      std::cout << future.get() << std::endl;
	    future.get();
    }
	TraceX(value);
    return 0;
}

void test_TerminateHandler() {
	set_abort_handler();
	set_terminate([](){
		log_trace << "This is the original terminate handler";
		std::abort();
	});

	log_trace << "Try to setup (expecting init):";
	setTerminateHandler();
	log_trace << "Try to setup twice (expecting nothing):";
	setTerminateHandler();
	TerminateHandler compilerErrorExpected;

//	queuedHandler();  // this is not compilable - as expected
}

void test_TerminateHandler2() {
	set_abort_handler();
	set_terminate([](){
		log_trace << "This is the original terminate handler";
		std::abort();
	});

	log_trace << "Try to setup (expecting init):";
	setTerminateHandler2();
	log_trace << "Try to setup twice (expecting nothing):";
	setTerminateHandler2();
//	TerminateHandler2 compilerErrorExpected;
//	queuedHandler();  // this is not compilable - as expected
}

int main() {
//	test1();
//	test_TerminateHandler();
	test_TerminateHandler2();

	log_trace << "Gonna terminate now";
//	terminate();
	test_ConcurrentTerminate();

	return 0;
}

auto testLocalStaticAtomic() {
	static std::atomic<long> x;
	return x++;
}
