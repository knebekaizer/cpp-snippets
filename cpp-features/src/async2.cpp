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

int main() {
    test();
    return 0;
}
