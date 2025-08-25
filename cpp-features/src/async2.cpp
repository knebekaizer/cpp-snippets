#include <thread>
#include <future>
#include <chrono>
#include <vector>
#include <string.h>

#include "trace.hpp"
#include "require.hpp"

using namespace std;
using namespace std::chrono_literals;

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
        return 777;
    });
    log_trace << time() << " waiting for the future, f.valid() == " << f.valid();
    int n = f.get();
    log_trace << time() << " future.get() returned with " << n << "; f.valid() = " << f.valid();
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

void asyncTest_4() {
    cout << endl;
    log_trace << "Use promise to propagate exception from a thread";
    std::promise<int> p;
    std::future<int> f = p.get_future();

    std::thread t([&p]{
        try {
            // code that may throw
            throw std::runtime_error("Exception example");
        } catch(...) {
            try {
                // store anything thrown in the promise
                p.set_exception(std::current_exception());
            } catch(...) {} // set_exception() may throw too
        }
    });

    try {
        std::cout << f.get();
    } catch(const std::exception& e) {
        std::cout << "Exception from the thread: " << e.what() << '\n';
    }
    t.join();
}

void test_promise() {
    struct R {
        int v = -1;
    };
    TraceX(sizeof(promise<void>), sizeof(promise<char>), sizeof(promise<R>));

    promise<R> r;
//    r.set_value({42});
    r.set_exception(nullptr);
    TraceX(r.get_future().get().v);

	promise<int> prom;
	auto fut = prom.get_future();
	TraceX(fut.valid());
	log_trace << "Waiting for 3s...";
	fut.wait_for(3s);
	log_trace << "...done";
	TraceX(fut.valid());
	prom.set_value(42);
//    prom.set_exception(nullptr);
    TraceX(fut.get());

    try {
        throw std::runtime_error("Exception example");
    } catch (std::exception&) {
        try {
            // store anything thrown in the promise
            prom.set_exception(std::current_exception());
        } catch (std::exception& e) {  // set_exception() may throw too
            log_trace << "Expected exception thrown from promise::set_exception " << e.what();
        } catch (...) {}
    }

    TraceX((fut.valid() ? to_string(fut.get()) : "No state"));
//    TraceX(fut.get());
//    TraceX(fut.valid());

    promise<void> p;
    p.set_exception(std::current_exception());
    p.get_future().get();
}

void test_promise_async() {
    cout << endl;
    log_trace << "Use promise to propagate exception from a thread";
    std::promise<int> p;
    std::future<int> f = p.get_future();

    std::thread t([&p]{
        try {
            // code that may throw
            throw std::runtime_error("Exception example");
        } catch(...) {
            try {
                // store anything thrown in the promise
                p.set_exception(std::current_exception());
            } catch(...) {} // set_exception() may throw too
        }
    });

    try {
        std::cout << f.get();
    } catch(const std::exception& e) {
        std::cout << "Exception from the thread: " << e.what() << '\n';
    }
    t.join();
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

ostream& operator<<(ostream& os, const struct  S&);
struct S {
	static int uniq;
	int id;
	S() : id(++uniq) { TraceX(id); }
	S(S&& x) : id(x.uniq) { log_trace << "move> " << *this; }
	~S() { TraceX(id); }
};
int S::uniq = 0;
ostream& operator<<(ostream& os, const S& r) { return os << r.id; }

auto makeFuture() {
	auto prom = std::promise<S>{};
	{
//		auto fut = prom.get_future();
	}
	prom.set_value(S());
	auto fut = prom.get_future();
	TraceX(fut.valid());
	return fut;
}

void test_makeFuture() {
	auto fut = makeFuture();
	auto get = fut.get();
	TraceX(get);
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
    auto fun = [&max_async_count] {
        R r;
        int c = R::g_count;
        max_async_count = max(max_async_count, c);
        TraceX(r);
        std::this_thread::sleep_for(1s);
    };
    vector<future<void>> v;
    for (auto n = 10000u; --n;) {
        try {
            v.emplace_back(std::async(std::launch::async, fun));
        //        std::this_thread::sleep_for(10ms);
        } catch (std::exception& e) {
            log_error << e.what() << "; number of async threads: " << max_async_count;
            TraceX(v.size());
            throw;
        }
    }
}


//#include <sys/time.h>
#include <sys/resource.h>
#include <stdio.h>
ostream& operator<<(ostream& os, const rlimit& r) {
	return os << '{' << r.rlim_cur << ',' << r.rlim_max << '}';
}
bool operator==(const rlimit& a, const rlimit& b) {
    return a.rlim_cur == b.rlim_cur && a.rlim_max == b.rlim_max;
}
bool operator!=(const rlimit& a, const rlimit& b) {
    return !(a == b);
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
	ThreadLimit(rlim_t lim = GUESS_) : save_(get_()) {
		rlim_t rlim_cur = min(lim, save_.rlim_cur);
//		TraceX(save_, rlim_cur);
		rlimit tmp{.rlim_cur = rlim_cur, .rlim_max = save_.rlim_max};
		set_(tmp);
//		log_trace << "previous: " << save_ << "; current: " << get();
	}
	~ThreadLimit() {
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

ostream& operator<<(ostream& os, const future_status s) {
	return os << (s == future_status::ready ? "ready"
	    : s == future_status::timeout ? "timeout"
	    : s == future_status::deferred ? "deferred"
		: "unknown");

}

void test_ThreadLimitException() {
	try {
		ThreadLimit lim(10);
		auto fut = async(launch::async, [] { return; });
		auto status = fut.wait_for(1s); // shall not reach here
		log_trace << "fut.wait_for returns: " << status;
	} catch (exception& e) {
		log_trace << "Expected exception: " << e.what();
	}

	// however, when run with async | deferred it will run as deferred, instead of failure
	try {
		ThreadLimit lim(10);
		auto fut = async(launch::async | launch::deferred, [] { this_thread::sleep_for(3s); });
		auto status = fut.wait_for(1s);
		log_trace << "fut.wait_for returns: " << status;
	} catch (exception& e) {
		log_trace << "Expected exception: " << e.what();
	}
}

void test_ThreadLimit_scoped() {
    auto save = ThreadLimit::get();
    {
        ThreadLimit lim;
    }
    auto after = ThreadLimit::get();
    if (save != after)
        log_error << "ThreadLimit is not restored: save: " << save << "; after: " << after;

}

struct Uniq {
	int& count;
	explicit Uniq(int& c) : count(c) { ++count; }
	~Uniq() { --count; }
	Uniq() = delete;
	Uniq(const Uniq&) = delete;
	Uniq(Uniq&&) = delete;
};

struct Runnable {
	unique_ptr<Uniq> uniq;
	chrono::steady_clock::duration delay{3s};
	void operator()() { this_thread::sleep_for(delay); }
	explicit Runnable(unique_ptr<Uniq>&& u) : uniq{std::move(u)} { TraceX(uniq->count); }
	Runnable(Runnable&&) = default;
	Runnable& operator=(Runnable&&) = default;
	~Runnable() { if (uniq) TraceX(uniq->count); }
};

void test_functor() {
	int count = 0;
	{
		auto fut = async(launch::async, Runnable(make_unique<Uniq>(count)));
		this_thread::sleep_for(1ms);
		TraceX(count);
	}
	TraceX(count);

	struct C {
		C() { TraceF;}
		~C() { TraceF;}
	};
	auto prom = make_unique<promise<C>>();
	auto fut = prom->get_future();
	TraceX(1);
	prom->set_value(C());
	TraceX(2);
	prom.reset();
	TraceX(21);
	{
		fut.get();
	}
	TraceX(3);
}

int main() try {
	test();
//	test_ThreadLimit_scoped();
	test_promise();
	//	test_promise2();
	//	test_async_deferred();
	test_ThreadLimitException();
	return 0;
}
catch (const std::exception& e) {
	log_error << "[FAILED] " << e.what();
	return -1;
}
