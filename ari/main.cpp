#include <iostream>
#include <stdint.h>
#include <stdlib.h>
#include "trace.h"
//#include "features.h"

#include <vector>
#include <tuple>
#include "gsl-lite.hpp"

#include <utility>
#include <thread>
#include <chrono>

#include <signal.h>

using namespace std;

mutex logMutex;

static void sigStopHandler(int signum)
{
//    std::this_thread::get_id()
    {
        const std::lock_guard<std::mutex> lock(logMutex);
        log_trace "Thread " << this_thread::get_id() << "; Signal " << signum;
    }
    /* Take appropriate actions for signal delivery */
}


void setStopHandler()
{
    struct sigaction sa;
    sa.sa_handler = sigStopHandler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART; /* Restart functions if
                                 interrupted by handler */
    if (sigaction(SIGUSR2, &sa, NULL) == -1)
        log_error << "sigaction failed";

    /* Further code */
}


using bit = int8_t;
constexpr bit Zero = 0;
constexpr bit One = 1;

using Buf = vector<int8_t>;

long sum;

void print(const Buf& buf) {
//    for (bit x : buf) cout << (int)x;
//    cout << endl;
    sum += buf[buf.size()/2];
}

void bits(int n, int k, Buf& buf) {
    assert(k <= n);
    assert(n <= buf.size());

    int offset = buf.size() - n;
     if (k == n) {
        ::std::fill(buf.begin() + offset, buf.end(), One);
        print(buf);
        fill(buf.begin() + offset, buf.end(), Zero);
        return;
    }
    if (k == 0) {
        fill(buf.begin() + offset, buf.end(), Zero);
        print(buf);
        return;
    }

    buf[offset] = Zero;
    bits(n - 1, k, buf);

    buf[offset] = One;
    bits(n - 1, k - 1, buf);
}

tuple<int, int> options(int argc, char* argv[]) {
    assert(argc == 3);
    return {atoi(argv[1]),
        atoi(argv[2])};
}

void testBits(int n, int k) {
    Buf buf(n);
    bits(n, k, buf);
//    TraceX(sum); cout << flush;
}

void runTest(int n, int k, int nThr) {
    {
        const std::lock_guard<std::mutex> lock(logMutex);
        log_trace << "thread " << nThr << " started";
        cout << flush;
    }
    testBits(n, k);
    {
        const std::lock_guard<std::mutex> lock(logMutex);
        log_trace << "thread " << nThr << " finished";
        cout << flush;
    }
}



void testMthr() {
    using Threads = vector<thread>;
    Threads threads;
    for (int k = 0; k != 8; ++k) {
        threads.emplace_back(runTest, (k+1)*32, 3, k);
    }
    {
        const std::lock_guard<std::mutex> lock(logMutex);
        log_trace << "Sending signal to " << threads[6].get_id();
    }
    pthread_kill(threads[6].native_handle(), SIGUSR2);
    for (auto& t : threads) t.join();
}

template <typename T, typename... Args>
unique_ptr<T> make_unique_(Args&&... args) {
    return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}

class NoCopy {
public:
    NoCopy(NoCopy const&) = delete;
    NoCopy(NoCopy&&) = default;
    NoCopy() = default;
};
struct A {
    A(NoCopy&& nc) : nc_(forward<NoCopy>(nc)) {}
    NoCopy nc_;
};

int main(int argc, char* argv[]) {
	log_info << "Start";

	NoCopy nc;
	auto x = make_unique_<A>(move(nc));
    setStopHandler();

//    auto [n, k] = options(argc, argv);
//    TraceX(n, k);
//    testBits(n, k);
    testMthr();

	return 0;
}
