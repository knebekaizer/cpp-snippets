/// @file Play with async policy.

#include <thread>
#include <future>
#include <iostream>
#include <string>
#include <chrono>

#include "trace.hpp"

using namespace std::chrono_literals;
using namespace std;

extern std::string Backtrace(int skip = 1);

#define log_thread log_trace << '[' << std::this_thread::get_id() << "] "

//ostream& operator<<(ostream& os, future_status x) {
//    return os << (
//        x == future_status::ready ? "ready"
//        : x == future_status::timeout ? "timeout"
//        : x == future_status::deferred ? "deferred"
//        : "unknown");
//}

namespace {
using namespace std::chrono;
struct Timer {
    using Clock = std::chrono::steady_clock;
//    using T = result_of_t<decltype(&Clock::now)()>; // time_point<Clock, duration<long, std::ratio<1, 1000'000'000>>> ?
    Timer() : start(Clock::now()) {}
    Clock::time_point start;

    auto elapsed() const { return duration_cast<microseconds>(Clock::now() - start).count(); }
    auto reset() {
        auto now = Clock::now();
        auto elapsed = now - start;
        start = now;
        return elapsed.count();
    }
};

ostream& operator<<(ostream& os, future_status status) {
    switch (status) {
        case std::future_status::deferred: return os << "deferred";
        case std::future_status::timeout: return os << "timeout";
        case std::future_status::ready: return os << "ready";
        default: return os << "unknown";
    }
}

} // namespace

void future_async_dtor() {
    auto policy = std::launch::async;
    log_thread << "Main thread, policy `async`: " << (int)policy;
    Timer t;
    {
        auto fut = std::async(
            policy,
            [&t](auto delay) {
                log_thread << "Worker starts now; time: " << t.elapsed();
                std::this_thread::sleep_for(delay);
                log_thread << "Worker finished; time: " << t.elapsed();
            },
            3s);
        log_thread << "future::valid: " << fut.valid();
        log_thread << "Main thread sleep for 1 s to allow worker first; time: " << t.elapsed();
        std::this_thread::sleep_for(1s);
        log_thread << "Main thread blocks at the future dtor until the shared state is ready; time: " << t.elapsed();
    }
    log_thread << "Main thread unblocked now; time: " << t.elapsed();
    cout << endl;
}

void future_deferred_get() {
    auto policy = std::launch::deferred;
    log_thread << "Main thread, policy `deferred`: " << (int)policy;
    Timer t;
    {
        auto fut = std::async(
            policy,
            [&t](auto delay) {
                log_thread << "Worker starts now; time: " << t.elapsed();
                std::this_thread::sleep_for(delay);
                log_thread << "Worker finished; time: " << t.elapsed();
            },
            3s);
        log_thread << "Main thread sleep for 1 s to allow worker first; time: " << t.elapsed();
        std::this_thread::sleep_for(1s);
        log_thread << "Main thread blocks at the future dtor until the shared state is ready; time: " << t.elapsed();
        // launch::async policy: Does not block on the dtor
        // launch::deferred is lasy: does not call the callback if there is no fut.get
        log_thread << "Main thread is waiting for future::get(); time: " << t.elapsed();
        log_thread << "future::valid: " << fut.valid();
        fut.get();
        log_thread << "future::valid: " << fut.valid();
    }
    log_thread << "Main thread unblocked now; time: " << t.elapsed();
    cout << endl;
}

void future_deferred_wait() {
    auto policy = std::launch::deferred;
    log_thread << "Main thread, policy `deferred`: " << (int)policy;
    Timer t;
    {
        auto fut = std::async(
            policy,
            [&t](auto delay) {
                log_thread << "Worker starts now; time: " << t.elapsed();
                std::this_thread::sleep_for(delay);
                log_thread << "Worker finished; time: " << t.elapsed();
            },
            3s);
        log_thread << "Main thread sleep for 1 s to allow worker first; time: " << t.elapsed();
        std::this_thread::sleep_for(1s);
        log_thread << "Main thread blocks at the future dtor until the shared state is ready; time: " << t.elapsed();
        // launch::async policy: Does not block on the dtor
        // launch::deferred is lasy: does not call the callback if there is no fut.get
        log_thread << "Main thread is waiting for future::get(); time: " << t.elapsed();
        log_thread << "future::valid: " << fut.valid() << "; time: " << t.elapsed();
        log_thread << "run wait_for:";
        auto status = fut.wait_for(1s);
        log_thread << "status: " << status << "; time: " << t.elapsed();
        log_thread << "run wait:";
        fut.wait();
        log_thread << "future::valid: " << fut.valid() << "; time: " << t.elapsed();
        log_thread << "run get:";
        fut.get();
        log_thread << "future::valid: " << fut.valid() << "; time: " << t.elapsed();
    }
    log_thread << "Main thread unblocked now; time: " << t.elapsed();
    cout << endl;
}

void future_deferred_dtor() {
    auto policy = std::launch::deferred;
    log_thread << "Main thread, policy `deferred`: " << (int)policy;
    Timer t;
    {
        auto fut = std::async(
            policy,
            [&t](auto delay) {
                log_thread << "Worker starts now; time: " << t.elapsed();
                std::this_thread::sleep_for(delay);
                log_thread << "Worker finished; time: " << t.elapsed();
            },
            3s);
        log_thread << "future::valid: " << fut.valid();
        log_thread << "Main thread sleep for 1 s to allow worker first; time: " << t.elapsed();
        std::this_thread::sleep_for(1s);
        log_thread << "Main thread DOES NOT block at the future dtor because not async and deferred; time: " << t.elapsed();
    }
    log_thread << "Deferred callback never called  time: " << t.elapsed();
    cout << endl;
}

void future_async_deferred_dtor() {
    auto policy = std::launch::async | std::launch::deferred;
    log_thread << "Main thread, policy: " << (int)policy;
    Timer t;
    {
        auto fut = std::async(
            policy,
            [&t](auto delay) {
                log_thread << "Worker starts now; time: " << t.elapsed();
                std::this_thread::sleep_for(delay);
                log_thread << "Worker finished; time: " << t.elapsed();
            },
            3s);
        log_thread << "Main thread sleep for 1 s to allow worker first; time: " << t.elapsed();
        std::this_thread::sleep_for(1s);
        log_thread << "Main thread BLOCKS at the future dtor until the shared state is ready; time: " << t.elapsed();
    }
    log_thread << "Async+Deferred callback called; time: " << t.elapsed();
    cout << endl;
}


void future_async_wait_for() {
    auto policy = std::launch::async;
    log_thread << "Main thread, policy `async`: " << (int)policy;
    Timer t;
    {
        auto fut = std::async(
            policy,
            [&t](auto delay) {
                log_thread << "Worker starts now; time: " << t.elapsed();
                std::this_thread::sleep_for(delay);
                log_thread << "Worker finished; time: " << t.elapsed();
            },
            3s);
        log_thread << "Main thread blocks at the future wait_for until the shared state is ready; time: " << t.elapsed();
        duration<uint8_t> delay = -1s;
        auto status = fut.wait_for(delay);
        log_thread << "Ends with the status: " << status << ", time: " << t.elapsed();
    }
    log_thread << "Main thread unblocked now; time: " << t.elapsed();
    cout << endl;
}

int main() {
    cout << boolalpha;
//    future_async_dtor();
//    future_deferred_dtor();
//    future_deferred_get();
    future_deferred_wait();
//    future_async_deferred_dtor();
//    future_async_wait_for();
	return 0;
}
