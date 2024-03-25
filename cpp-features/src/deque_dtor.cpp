#include "trace.hpp"
#include <array>
#include <iostream>
#include <deque>
#include <typeinfo>
#include <set>
#include <functional>

size_t num_constructed;
size_t num_destroyed;
size_t num_alloc;
size_t num_dealloc;
size_t num_dtor;

struct S {
	S() { log_trace << "Default ctor " << typeid(S).name() << ": " << (void*)this; }
	S(const S&) { log_trace << "Copy ctor " << (void*)this; }
	S(S&&) { log_trace << "Move ctor " << (void*)this; }
	S& operator=(const S&) { log_trace << "Copy assignment " << (void*)this; return *this; }
	S& operator=(S&&) { log_trace << "Move assignment " << (void*)this; return *this; }
	~S() {
		++num_dtor;
		TraceX(this, num_dtor);
	}
	void operator delete(void *ptr) {
		TraceX(ptr);
		::operator delete(ptr);
	}
	void* operator new(size_t sz) {
		return malloc(sz);
	}
	unsigned char c_;
};

//template<class T = S>
//using Tp = S;
struct custom_allocator {
    using T = S;
    typedef T value_type;

    custom_allocator() = default;

//    template<typename T_rhs>
//    custom_allocator(const std::allocator<T_rhs>& rhs) { }

    value_type * allocate(std::size_t n) {
        value_type * p = static_cast<value_type *>(::operator new(n * sizeof(value_type)));
		++num_alloc;
	    TraceX(typeid(T).name(), sizeof(T), this, p, num_alloc);
	    return p;
    }

    void deallocate(value_type * p, std::size_t n) {
        ::operator delete(p);
//		delete (char*)p;
	    ++num_dealloc;
	    TraceX(typeid(T).name(), sizeof(T), this, p, num_dealloc);
    }
//    void construct(value_type * p, value_type value) {
//        new ((void *) p) value_type(value);
//        num_constructed++;
//	    TraceX(typeid(T).name(), sizeof(T), this, p, num_constructed);
//    }

    void construct(value_type * p) {
//        new ((void *) p) value_type;
        num_constructed++;
	    TraceX(typeid(T).name(), sizeof(T), this, p, num_constructed);
    }

    void destroy(value_type * p) {
        num_destroyed++;
        p->~value_type();
	    TraceX(typeid(T).name(), sizeof(T), this, p, num_destroyed);
	}
	template <class U>
	struct rebind { typedef std::allocator<U> other; };
};

template<class T = S>
struct alloc2 {
    typedef T value_type;

//    template<typename T_rhs>
//    custom_allocator(const std::allocator<T_rhs>& rhs) { }

    value_type * allocate(std::size_t n) {
        value_type * p = static_cast<value_type *>(::operator new(n * sizeof(value_type)));
		++num_alloc;
	    TraceX(typeid(T).name(), sizeof(T), this, p, num_alloc);
	    return p;
    }

    void deallocate(value_type * p, std::size_t n) {
        ::operator delete(p);
//		delete (char*)p;
	    ++num_dealloc;
	    TraceX(typeid(T).name(), sizeof(T), this, p, num_dealloc);
    }
//    void construct(value_type * p, value_type value) {
//        new ((void *) p) value_type(value);
//        num_constructed++;
//	    TraceX(typeid(T).name(), sizeof(T), this, p, num_constructed);
//    }

    void construct(value_type * p) {
//        new ((void *) p) value_type;
        num_constructed++;
	    TraceX(typeid(T).name(), sizeof(T), this, p, num_constructed);
    }

    void destroy(value_type * p) {
        num_destroyed++;
        p->~value_type();
	    TraceX(typeid(T).name(), sizeof(T), this, p, num_destroyed);
	}
	template <class U>
	struct rebind { typedef std::allocator<U> other; };
};

void test_deque() {
		std::deque<S, custom_allocator> d1(1);
}

void test_set() {
	TraceF;
	auto less = [](const S& s1, const S& s2) { return s1.c_ < s2.c_; };

	std::set<S, decltype(less), alloc2<S>> d1;
	d1.emplace();
	d1.emplace();
}

int main() {
	test_deque();
	test_set();
}