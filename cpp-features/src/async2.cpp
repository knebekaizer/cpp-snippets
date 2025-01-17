#include <thread>
#include <future>
#include <chrono>
#include <vector>
#include <string.h>

#include "trace.hpp"
#include "require.hpp"

using namespace std;

string time() {
    static auto start = std::chrono::steady_clock::now();
    std::chrono::duration<double> d = std::chrono::steady_clock::now() - start;
    return "[" + std::to_string(d.count()) + "s]";
}

void test() {
    TraceX( std::this_thread::get_id() );
    log_trace << time() << " launching thread";
    std::future<int> f = std::async(std::launch::async, []{
        TraceX( std::this_thread::get_id() );
        std::this_thread::sleep_for(1s);
        return 7;
    });
    log_trace << time() << " waiting for the future, f.valid() == " << f.valid();
    int n = f.get();
    log_trace << time() << " future.get() returned with " << n << ". f.valid() = " << f.valid();
}

void test_async_deferred() {
    TraceX( std::this_thread::get_id() );
    log_trace << time() << " launching thread";
    std::future<int> f = std::async(std::launch::async | std::launch::deferred, [&]{
        TraceX( std::this_thread::get_id() );
        std::this_thread::sleep_for(1s);
        return 7;
    });
	std::this_thread::sleep_for(10ms);
	log_trace << time() << " waiting for the future, f.valid() == " << f.valid();
    int n = f.get();
    log_trace << time() << " future.get() returned with " << n << ". f.valid() = " << f.valid();
}

void test_promise() {
	promise<int> prom;
	auto fut = prom.get_future();
	TraceX(fut.valid());
	log_trace << "Waiting for 5s...";
	fut.wait_for(5s);
	log_trace << "...done";
	TraceX(fut.valid());
	prom.set_value(42);
	TraceX(fut.get());
	TraceX(fut.valid());
}

void test_promise2() {
	auto prom = std::promise<int>{};
	{
//		auto fut = prom.get_future();
	}
	prom.set_value(21);
	auto fut = prom.get_future();
	TraceX(fut.get(), fut.valid());

}


struct R {
	static atomic<int> g_count;
	int count = ++g_count;
	~R() { --g_count; }
};
atomic<int> R::g_count{0};
ostream& operator<<(ostream& os, const R& r) { return os << r.count; }

void test_threadLimit() {
    int max_async_count = 0;
    auto fun = [&max_async_count]{
        R r;
        int c = R::g_count;
        max_async_count = max(max_async_count, c);
        TraceX(r);
        std::this_thread::sleep_for(1s);
    };
    vector<future<void>> v;
    try {
        for (auto n = 10000u; --n;) {
            v.emplace_back(std::async(std::launch::async, fun));
            //        std::this_thread::sleep_for(10ms);
        }
    } catch (std::exception& e) {
        throw;
        log_error << e.what() <<"; number of async threads: " << max_async_count;
        TraceX(v.size());
    }
//    log_trace << time() << " waiting for the future, f.valid() == " << f.valid();
//    log_trace << time() << " future.get() returned with " << n << ". f.valid() = " << f.valid();
}


//#include <sys/time.h>
#include <sys/resource.h>
#include <stdio.h>
ostream& operator<<(ostream& os, const rlimit& r) {
	return os << '{' << r.rlim_cur << ',' << r.rlim_max << '}';
}

class ThreadLimit {
	rlimit save_ = {0};
public:
//#ifdef RLIMIT_NTHR
#ifdef __QNX__
	static const auto RSRC = RLIMIT_NTHR;
	static const rlim_t GUESS_ = 5;
#else // Linux
	static const auto RSRC = RLIMIT_NPROC;
	static const rlim_t GUESS_ = 5000;
#endif
	rlim_t rlim_cur;
	ThreadLimit(rlim_t lim = GUESS_)
		: save_(get_())
		, rlim_cur(min(lim, save_.rlim_cur))
	{
		TraceX(save_, rlim_cur);
		rlimit tmp{.rlim_cur = rlim_cur, .rlim_max = save_.rlim_max};
		TraceX(tmp);
		set_(tmp);
//		set_({.rlim_cur = rlim_cur, .rlim_max = save_.rlim_max});
		tmp = get_();  // check the actual
		TraceX(tmp);
		require(tmp.rlim_cur == rlim_cur) << "Failed to set requested limit: " << lim
		                                  << "; Result: " << tmp;
	}
	~ThreadLimit() {
		TraceF;
		set_(save_);
	}

private:
	rlimit get_() {
		rlimit tmp = {0};
		int err = getrlimit(RSRC, &tmp);
		require(err == 0) << "getrlimit failed: " << strerror(errno);
		TraceX(tmp);
		return tmp;
	}
	void set_(rlimit lim) {
		TraceX(lim);
		int err = setrlimit(RSRC, &lim);
		require(err == 0) << "setrlimit failed: " << strerror(errno);
	}

};

void test_limit(int rmax = -1) {
    struct rlimit rlim;
    int err = getrlimit(RLIMIT_NPROC, &rlim);
	require(err == 0) << "getrlimit failed: " << strerror(errno);
    TraceX(1, err, rlim);
    if (rmax <= 0) {
        rlim.rlim_cur -= 1;
        rlim.rlim_max -= 1;
    } else {
        rlim.rlim_cur = rmax;
//        rlim.rlim_max = rmax;
    }
    err = setrlimit(RLIMIT_NPROC, &rlim);
	require(err == 0) << "setrlimit(" << RLIMIT_NPROC << ", " << rlim << ") failed: " << strerror(errno);
	TraceX(2, err, rlim);
    err = getrlimit(RLIMIT_NPROC, &rlim);
    TraceX(3, err, rlim);
}

int main() try {
    test();
	{
		ThreadLimit lim;
	}
	test_limit(5000);
	test_threadLimit();
	test_promise();
	test_promise2();
	test_async_deferred();
    return 0;
}
//catch (const err::tr_error& e) {
//	log_error << "[FAILED] " << e.what() << "\nat " << e.where();
//	return -1;
//}
catch (const std::exception& e) {
	log_error << "[FAILED] " << e.what();
	return -1;
}
