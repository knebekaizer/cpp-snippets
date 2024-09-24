/// @file std::future & exception propagation.

#include <thread>
#include <future>
#include <iostream>
#include <string>
#include <chrono>

#include "trace.hpp"

using namespace std::chrono_literals;
using namespace std;

extern std::string Backtrace(int skip = 1);

string time() {
	static auto start = std::chrono::steady_clock::now();
	std::chrono::duration<double> d = std::chrono::steady_clock::now() - start;
	return "[" + std::to_string(d.count()) + "s]";
}

void asyncTest_1() {
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

void asyncTest_2() {
	log_trace << time() << " launching thread";
	std::future<int> f = std::async(std::launch::async, []{
		std::this_thread::sleep_for(1s);
		TraceX( std::this_thread::get_id() );
		throw std::runtime_error("7");
		return 7;
	});
	log_trace << time() << " waiting for the future, f.valid() == " << f.valid();
	try {
		int n = f.get();
		log_trace << time() << " future.get() returned with " << n << ", f.valid() = " << f.valid() ;
	} catch(const std::exception& e) {
		log_trace << time() << " caught exception " << e.what() << ", f.valid() == " << f.valid();
	}
}

void asyncTest_3() {
	// future from a packaged_task
	cout << endl;
	log_trace << "async vs promise+thread:";
	std::packaged_task<int()> task([]{
		log_trace << "This is a packaged_task: current thread_id = " << this_thread::get_id();
		std::this_thread::sleep_for(1s);
		return 7; }); // wrap the function
	std::future<int> f1 = task.get_future();  // get a future
	std::thread t(std::move(task)); // launch on a thread
 
	// future from an async()
	std::future<int> f2 = std::async(std::launch::async, []{
		std::this_thread::sleep_for(2s);
		log_trace << "This runs as async(): current thread_id = " << this_thread::get_id();
		return 8; });
 
	// future from a promise
	std::promise<int> p;
	std::future<int> f3 = p.get_future();
	std::thread( [&p]{
		std::this_thread::sleep_for(5s);
		log_trace << "Running as `promise::set_value_at_thread_exit`: current thread_id = " << this_thread::get_id();
		p.set_value_at_thread_exit(9); }).detach();
 
	log_trace << "Waiting..." << std::flush;
	f1.wait();
	log_trace << "Waiting... 1" << std::flush;
	f2.wait();
	log_trace << "Waiting... 2" << std::flush;
	f3.wait();
	log_trace << "Waiting... 3" << std::flush;
	log_trace << "...Done!\nResults are: "
			  << f1.get() << ' ' << f2.get() << ' ' << f3.get();
	t.join();	 
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

int main() {
	asyncTest_1();
	asyncTest_2();
	asyncTest_3();
	asyncTest_4();
	return 0;
}
