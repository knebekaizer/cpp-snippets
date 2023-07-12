#include "trace.hpp"
#include <array>
#include <iostream>
#include <deque>
#include <map>
#include <functional>

size_t actual_elements_destroyed;

struct S {
	S() { log_trace << "Default ctor " << (void*)this; }
	S(const S&) { log_trace << "Copy ctor " << (void*)this; }
	S(S&&) { log_trace << "Move ctor " << (void*)this; }
	S& operator=(const S&) { log_trace << "Copy assignment " << (void*)this; return *this; }
	S& operator=(S&&) { log_trace << "Move assignment " << (void*)this; return *this; }
	~S() { log_trace << "S dtor " << (void*)this; }
	void operator delete(void *ptr) {
		TraceX(ptr);
		::operator delete(ptr);
	}
	void* operator new(size_t sz) {
		return malloc(sz);
	}
};

template<class Tp = S>
//using Tp = S;
struct custom_allocator {
    typedef Tp value_type;

    custom_allocator() = default;

//    template<typename T_rhs>
//    custom_allocator(const std::allocator<T_rhs>& rhs) { }

    value_type * allocate(std::size_t n) {
        value_type * p = static_cast<value_type *>(::operator new(n * sizeof(value_type)));
        return p;
    }

    void deallocate(value_type * p, std::size_t n) {
//        ::operator delete(p);
		delete p;
//        memory_deallocated = true;
    }
    void construct(value_type * p, value_type value) {
        new((void *) p)value_type(value);
//        actual_elements_constructed++;
    }

    void destroy(value_type * p) {
		TraceX(1, (void*)p, actual_elements_destroyed);
        actual_elements_destroyed++;
        p->~value_type();
	    TraceX(2, (void*)p, actual_elements_destroyed);
    }
	template <class U>
	struct rebind { typedef std::allocator<U> other; };
};

void test_types() {
	{
//		std::deque<S, custom_allocator<S>> d1(1);
		std::deque<S> d1(1);
	}
	{
//		std::map<int, S, custom_allocator<S>> d1({0, {}});
//		std::map<int, S,> d1;
//		d1.emplace(0, S());
//		TraceF;
	}
}

int main() {
    test_types();
}