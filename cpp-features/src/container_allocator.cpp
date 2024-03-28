#include "trace.hpp"
#include <array>
#include <iostream>
#include <deque>
#include <list>
#include <set>
#include <typeinfo>
#include <functional>

using namespace std;

static struct Stat {
	size_t nConstruct = 0;
	size_t nDestroy = 0;
	size_t nAlloc = 0;
	size_t nDealloc = 0;
} stat;

std::ostream& operator<<(ostream& os, Stat const& s) {
	return os <<
		"nAlloc: " << s.nAlloc << "; "
		"nDealloc: " << s.nDealloc << "; "
		"nConstruct: " << s.nConstruct << "; "
		"nDestroy: " << s.nDestroy;
}


struct S {
	S() { log_trace << "Default ctor " << typeid(S).name() << ": " << (void*)this; }
	S(const S&) { log_trace << "Copy ctor " << (void*)this; }
	S(S&&) { log_trace << "Move ctor " << (void*)this; }
	S& operator=(const S&) { log_trace << "Copy assignment " << (void*)this; return *this; }
	S& operator=(S&&) { log_trace << "Move assignment " << (void*)this; return *this; }
	~S() {
        ++stat.n_dtor;
		TraceX(this, stat.n_dtor);
	}
	void operator delete(void *ptr) {
		TraceX(ptr);
		::operator delete(ptr);
	}
	void* operator new(size_t sz) {
		return malloc(sz);
	}
	unsigned char c_;
    static struct Stat {
        size_t n_ctor;
        size_t n_dtor;
    } stat;
};
S::Stat S::stat;

struct alloc1 {
    using T = S;
    typedef T value_type;

    alloc1() = default;

    value_type * allocate(std::size_t n) {
        value_type * p = static_cast<value_type *>(::operator new(n * sizeof(value_type)));
		++stat.nAlloc;
	    TraceX(typeid(T).name(), sizeof(T), this, p, stat.nAlloc);
	    return p;
    }

    void deallocate(value_type * p, std::size_t n) {
        ::operator delete(p);
	    ++stat.nDealloc;
	    TraceX(typeid(T).name(), sizeof(T), this, p, stat.nDealloc);
    }

    void construct(value_type * p) {
        ::new ((void *) p) value_type;
	    stat.nConstruct++;
	    TraceX(typeid(T).name(), this, p, stat.nConstruct);
    }

    void destroy(value_type * p) {
	    stat.nDestroy++;
        p->~value_type();
	    TraceX(typeid(T).name(), sizeof(T), this, p, stat.nDestroy);
	}

	static struct Stat_ : Stat {} stat;
};

template<class T = S>
struct alloc2 {
    typedef T value_type;

	alloc2() = default;

    value_type* allocate(std::size_t n) {
        value_type* p = static_cast<value_type *>(::operator new(n * sizeof(value_type)));
		++stat.nAlloc;
	    TraceX(typeid(T).name(), this, p, stat.nAlloc);
	    return p;
    }

    void deallocate(value_type * p, std::size_t n) {
        ::operator delete(p);
	    ++stat.nDealloc;
	    TraceX(typeid(T).name(), this, p, stat.nDealloc);
    }

//	template <class _Up, class... _Args>
	template <class... _Args>
	void construct(T* p, _Args&&... args) {
		::new ((void*)p) T(std::forward<_Args>(args)...);
		stat.nConstruct++;
		TraceX(typeid(T).name(), this, p, stat.nConstruct);
	}

    void destroy(value_type * p) {
	    stat.nDestroy++;
        p->~value_type();
	    TraceX(typeid(T).name(), this, p, stat.nDestroy);
	}

	template <class U>
	struct rebind { typedef alloc2<U> other; };

	template<typename U>
	alloc2(const alloc2<U>& other) {
		TraceF << "rebind allocator from the type " << typeid(U).name() << " to the type " << typeid(T).name();
	}

    static struct Stat_ : Stat {} stat;
};

template <typename T> typename alloc2<T>::Stat_ alloc2<T>::stat;

template <class Alloc>
void test_deque() {
	cout << endl;
	TraceF << typeid(Alloc).name();
	stat = {0};
	{
		std::deque<S, Alloc> d1(1);
	}
	log_trace << alloc2<S>::stat;
    log_trace << alloc2<S*>::stat;
}


void test_set() {
	cout << endl;
	TraceF;
	stat = {0};
	auto less = [](const S& s1, const S& s2) { return s1.c_ < s2.c_; };

	using Alloc = alloc2<S>; // std::allocator<S>;
	{
		Alloc a;
		std::set<S, decltype(less), Alloc> d1(less, a);
		d1.emplace();
		d1.emplace();
	}
	log_trace << alloc2<S>::stat;
}

template <class Alloc>
void test_list() {
	cout << endl;
	TraceF << typeid(Alloc).name();
	stat = {0};
	{
		std::list<S, Alloc> d1;
		d1.emplace_back();
		d1.emplace_back();
	}
	log_trace << Alloc::stat;
}

int main() {
//	test_deque<alloc1>();
	test_deque<alloc2<S>>();
	test_set();

	test_list<alloc1>();
	test_list<alloc2<S>>();
}