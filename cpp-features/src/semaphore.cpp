//
// Created by Vladimir on 2023-07-28.
//

#include <semaphore>
#include <thread>
#include <queue>

#include "trace.hpp"

using T = int;
class Sink {
public:
	Sink();
	void sink(T&& x);
private:
	void slowDown_();
//	std::thread worker_;
//	std::binary_semaphore blocker_;
};

class Queue {
public:
	Queue(Sink& s);
	void push(T&& x);
	void stop();
private:
	void handle_();
	static constexpr size_t maxSize = 8;
	Sink& sink_;
	std::queue<T> buf_;
	std::thread worker_;
	std::counting_semaphore<> availableElements_;
	std::counting_semaphore<> freeSlots_;
	std::atomic<bool> running_ = false;
};

using namespace std;

Sink::Sink()
//	: worker_(&Sink::slowDown_, this)
//	, blocker_(1)
{
}

void Sink::slowDown_() {
	using namespace std::chrono_literals;
//	blocker_.acquire();
	log_trace << "sink blocking";
	this_thread::sleep_for(100ms);
//	blocker_.release();
	log_trace << "sink available";
}

void Sink::sink(T&& x) {
	TraceX(x);
	slowDown_();
}

Queue::Queue(Sink& s)
	: sink_(s)
	, buf_()
	, worker_(&Queue::handle_, this)
	, availableElements_(0)
	, freeSlots_(maxSize - 1)
{
}

void Queue::handle_() {
	log_trace << "async queue started";
	running_ = true;
	while (true) {
		log_trace << "checking for available elements: buf_.size = " << buf_.size();
		availableElements_.acquire();
		if (!running_ && buf_.empty()) break; // stopped
		log_trace << "elements available: buf_.size = " << buf_.size();
		sink_.sink(std::move(buf_.front()));
		buf_.pop();
		log_trace << "releasing one free slot: buf_.size = " << buf_.size();
		freeSlots_.release();
	}
	log_trace << "async queue stopped";
}

void Queue::push(T&& x) {
	log_trace << "waiting for free slots: buf_.size = " << buf_.size() << "; value = " << x;
	freeSlots_.acquire();
	log_trace << " free slot available: buf_.size = " << buf_.size();
	buf_.push(std::move((x)));
	log_trace << "signalling available element: buf_.size = " << buf_.size();
	availableElements_.release();
}

void Queue::stop() {
	TraceF;
//	while (!buf_.empty()) { this_thread::sleep_for(1ms); }
	log_trace << "stopping the worker thread";
	running_ = false;
	// release one extra time to unblock worker thread
	availableElements_.release();
	if (worker_.joinable())
		worker_.join();
	log_trace << "finishing...";
}

#include <semaphore.h>
#include <future>
#include <iostream>
#include <vector>
#include <stdlib.h>
using namespace std;

class ThreadLimit {
    static struct initializer {
        const unsigned max_;
        initializer(unsigned limit) : max_(limit) {
            sem_init(&x_, 0, max_);
        }
        operator sem_t*() { return &x_; }
        sem_t x_;
    } sem_;
public:
    struct Guard {
        ~Guard() { sem_post(ThreadLimit::sem_); }
    };
    static int tryGetOrWait() { return sem_trywait(sem_); }
    static int getOrWait() { return sem_wait(sem_); }
    static int free() {
        int n = 0;
        return sem_getvalue(sem_, &n) ? -1 : n;
    }
    static int used() {
        int n = 0;
        return sem_getvalue(sem_, &n) ? -1 : (int)sem_.max_ - n;
    }
    friend struct ThreadLimit::Guard;
};

ThreadLimit::initializer ThreadLimit::sem_{10};
std::mutex coutMtx; // used to synchronize output

void threads_limit() {
    std::vector<future<void>> v;
    for (auto n = 20; --n;) {
        if (ThreadLimit::tryGetOrWait()) {
            if (errno == EAGAIN) {
                unique_lock lk(coutMtx);
                cout << "Main thread shall wait" << endl;
            }
            ThreadLimit::getOrWait(); // wait may block
        }
        try {
            v.emplace_back(std::async(std::launch::async, [](){
                ThreadLimit::Guard guard;
                unsigned delay = random() & 7;
                {
                    unique_lock lk(coutMtx);
                    cout << "Active threads: " << ThreadLimit::used() << "; free: " << ThreadLimit::free()
                    << "; Worker thread will sleep for " << delay << " sec" << endl;
                }
                sleep(delay);
            }));
        } catch (std::system_error& e) {
            cerr << "Exception `std::system_error` caught: " << e.what() << endl;
        }
    }
}

int main() {
    threads_limit();
    return 0;
	Sink sink;
	Queue queue(sink);

	int x = 0;
	while (x < 30) {
		queue.push(std::move(x++));
//		this_thread::sleep_for(50ms);
	}
	queue.stop();
	return 0;
}
