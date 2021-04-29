//
// Created by Vladimir Ivanov on 03.06.2020.
//

#include "policy.h"

#include <exception>
#include "trace.h"

using namespace std;

template <typename F>
// TODO: Use invoke_result without && when compiled with C++17
auto tryOrTerminate(F&& tryClause) -> typename std::result_of<F&&()>::type {
  try {
    return tryClause();
  } catch (...) {
    // A trick to terminate with unhandled exception. This will print a stack trace
    // and write to iOS crash log.
    log_trace << "Handling exception from tryClause";
    std::terminate();
  }
}

int foo(int x) {
	TraceX(x);
	if (x & 1) {
		log_trace << "gonna throw";
		throw runtime_error("Odd parameter");
	}
	log_trace << "done without error";
	return x * x;
}

void terminatePolicy() {
	tryOrTerminate( [](){ foo(4); });
	tryOrTerminate( [](){ foo(5); });
	log_trace << "Done.";
}

enum class ErrorAction { kBad, kGood, kThrow, kTerminate };

ostream& operator << (ostream& os, ErrorAction const& x) {
    static const char* names[] = {"kBad", "kGood", "kThrow", "kTerminate"};
    return os << names[(int)x];
}
template <ErrorAction> struct ErrorPolicy { operator bool() const { return false; } };

template <> ErrorPolicy<ErrorAction::kGood>::operator bool() const { return true; }
template <> ErrorPolicy<ErrorAction::kThrow>::operator bool() const { throw runtime_error("Ba-bahh!"); }
template <> ErrorPolicy<ErrorAction::kTerminate>::operator bool() const { terminate(); }

template <ErrorAction action>
bool doSmth() {
    TraceX(action);
    return ErrorPolicy<action>();
}

template <ErrorAction action> bool doSpec(bool x) {  TraceX(x); return true; };
template <> bool doSpec<ErrorAction::kTerminate>(bool x) { TraceX(x); if (x) return x; else terminate(); };


void testPolicy() {

    bool ret = doSmth<ErrorAction::kGood>();
    TraceX(ErrorAction::kGood, ret);

    ret = doSmth<ErrorAction::kBad>();
    TraceX(ErrorAction::kBad, ret);

    try {
        ret = doSmth<ErrorAction::kThrow>();
        TraceX(ErrorAction::kThrow, ret);
    } catch (exception& e) {
        TraceX(e.what());
    }

    doSpec<ErrorAction::kTerminate>(true);
    doSpec<ErrorAction::kTerminate>(false);


    try {
        ret = doSmth<ErrorAction::kTerminate>();
        TraceX(ErrorAction::kTerminate, ret);
    } catch (exception& e) {
        TraceX(e.what());
    }
    log_trace << "Should not com here";
}


int main() {
	log_info << "Start";

//    terminatePolicy();

    testPolicy();

	return 0;
}
