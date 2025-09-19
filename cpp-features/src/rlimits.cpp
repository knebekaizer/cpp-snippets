//
// Created by vdi on 15.09.25.
//
#include <iostream>
#include <string>
#include "trace.hpp"
#include "require.hpp"

//#include <sys/time.h>
#include <sys/resource.h>
#include <stdio.h>
#include <string.h>

using namespace std;

ostream& operator<<(ostream& os, const rlimit& r) {
	return os << '{' << r.rlim_cur << ',' << r.rlim_max << '}';
}
bool operator==(const rlimit& a, const rlimit& b) {
	return a.rlim_cur == b.rlim_cur && a.rlim_max == b.rlim_max;
}
bool operator!=(const rlimit& a, const rlimit& b) {
	return !(a == b);
}

#ifdef __QNX__
static const auto RSRC_DEF = RLIMIT_NTHR;
#else // Linux
static const auto RSRC_DEF = RLIMIT_NPROC;
#endif

template <__rlimit_resource_t RSRC = RSRC_DEF>
class RLimit {
	rlimit save_ = {0};
public:
	//#ifdef RLIMIT_NTHR
#ifdef __QNX__
	static const rlim_t GUESS_ = 5;
#else // Linux
	static const rlim_t GUESS_ = 5000;
#endif
	RLimit(rlim_t lim = GUESS_) : save_(get_()) {
		rlim_t rlim_cur = min(lim, save_.rlim_cur);
		//		TraceX(save_, rlim_cur);
		rlimit tmp{.rlim_cur = rlim_cur, .rlim_max = save_.rlim_max};
		set_(tmp);
		//		log_trace << "previous: " << save_ << "; current: " << get();
	}
	~RLimit() {
		set_(save_);
	}

	static rlimit get() {
		rlimit tmp = {0};
		int err = getrlimit(RSRC, &tmp);
		require(err == 0) << "getrlimit failed: " << strerror(errno);
		return tmp;
	}

private:
	static rlimit get_() {
		rlimit tmp = get();
		//		TraceX(tmp);
		return tmp;
	}
	void set_(rlimit lim) {
		//		TraceX(lim);
		int err = setrlimit(RSRC, &lim);
		require(err == 0) << "setrlimit failed: " << strerror(errno);
		// check success
		rlimit tmp = get_();  // check the actual
		require(tmp.rlim_cur == lim.rlim_cur) << "Failed to set requested limit: " << lim
																			<< "; Result: " << tmp;
	}
};

int main() {
//	try {
	// RLimit<RLIMIT_DATA> lim(1000);
	RLimit<RLIMIT_AS> limAS(1000);
	// RLimit<RLIMIT_FSIZE> limFS(1000);
	// auto get = lim.get();
	// TraceX(get);
	auto p = malloc(100000);
	TraceX(p);
	// system("bash -c 'ulimit -a'");
	p = malloc(100000);
	TraceX(p);
	return 0;
}