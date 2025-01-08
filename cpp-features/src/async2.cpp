#include "trace.hpp"
#include <thread>
#include <future>
#include <chrono>

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

struct R {
    static atomic<int> g_count;
    int count = ++g_count;
    ~R() { --g_count; }
};
atomic<int> R::g_count = 0;
ostream& operator<<(ostream& os, const R& r) { return os << r.count; }

void test_threadLimit() {
    auto f = std::async(std::launch::async, []{
        R r;
        TraceX(r);
        std::this_thread::sleep_for(1s);
    });
    log_trace << time() << " waiting for the future, f.valid() == " << f.valid();
    int n = f.get();
    log_trace << time() << " future.get() returned with " << n << ". f.valid() = " << f.valid();
}

#include <sys/time.h>
#include <sys/resource.h>
ostream& operator<<(ostream& os, const rlimit& r) {
    return os << '{' << r.rlim_cur << ',' << r.rlim_max << '}';
}
void test_limit() {
    struct rlimit rlim;
    int err = getrlimit(RLIMIT_NPROC, &rlim);
    perror(0);
    TraceX(1, err, rlim);
    rlim.rlim_cur -= 1;
    rlim.rlim_max -= 1;
    err = setrlimit(RLIMIT_NPROC, &rlim);
    TraceX(2, err, rlim);
    err = getrlimit(RLIMIT_NPROC, &rlim);
    TraceX(3, err, rlim);
}

int main() {
    test();
    test_limit();
    return 0;
}
