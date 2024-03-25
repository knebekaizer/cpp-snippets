#include <trace.hpp>

#include <typeinfo>
#include <iostream>
#include <deque>

using namespace std;

int actual_elements_destroyed = 0;

class TestClass {
public:
	TestClass(int id) : m_id(id) {
		log_trace << "int ctor> " << (void*)this;
		++s_numConstructsCalled;
	}
	TestClass() : m_id(0) {
		log_trace << "default ctor> " << (void*)this;
		++s_numConstructsCalled;
	}
	TestClass(const TestClass& x) : m_id(x.m_id) {
		log_trace << "copy ctor> " << (void*)this;
		++s_numConstructsCalled;
	}
	TestClass(TestClass&& x) : m_id(x.m_id) {
		log_trace << "move ctor> " << (void*)this;
		++s_numConstructsCalled;
	}

	~TestClass() {
		log_trace << "dtor> " << (void*)this;
		++s_numDestructorsCalled;
	}

	int m_id;
	long long m_padding;
	static int s_numDestructorsCalled;
	static int s_numConstructsCalled;
};

int TestClass::s_numDestructorsCalled = 0;
int TestClass::s_numConstructsCalled = 0;

template<typename T>
class custom_allocator {
public:
	using value_type = T;
	using pointer = T*;
	using const_pointer = const T*;

	template<typename V> friend
	class custom_allocator;

	custom_allocator() {
		TraceF << "default ctor";
	};

	custom_allocator(const custom_allocator& x)
		: m_numAlloc(x.m_numAlloc), m_numDealloc(x.m_numDealloc)
	{
		TraceF << "copy ctor";
	};

	custom_allocator(custom_allocator&& x)
		: m_numAlloc(x.m_numAlloc), m_numDealloc(x.m_numDealloc)
	{
		TraceF << "move ctor";
	};

	template<typename U>
	custom_allocator(const custom_allocator<U>& other) : m_numAlloc(other.m_numAlloc), m_numDealloc(other.m_numDealloc) {
		TraceF << "rebind allocator from the type " << typeid(U).name() << " to the type " << typeid(T).name();
	}

//	template <class U>
//	struct rebind { typedef std::allocator<U> other; };

	T* allocate(std::size_t n) {
		++m_numAlloc;
		pointer p = static_cast<pointer>(::operator new(n * sizeof(T)));
		TraceX(typeid(T).name(), sizeof(T), this, m_numAlloc, n, p);
		return p;
	}

	void deallocate(T* p, std::size_t n) {
//		::operator delete((void*)p);
//		::operator delete(p);
		delete(p);
		++m_numDealloc;
		TraceX(typeid(T).name(), sizeof(T), this, m_numDealloc, n, p);
	}

//	template<typename U, typename... Args>
//	void construct(U* p, Args&& ... args) {
	template<typename... Args>
	void construct(T* p, Args&& ... args) {
		TraceX(typeid(T).name(), (void*)p);
		new(p) T(std::forward<Args>(args)...);
	}

//	template<typename U>
//	void destroy(U* p) {
	void destroy(T* p) {
		TraceX(typeid(T).name(), (void*)p);
		++actual_elements_destroyed;
		p->~T();
	}

//	template<typename U>
//	bool operator==(const custom_allocator<U>& other) const {
//		return m_numAllocations == other.m_numAllocations;
//	}
//
//	template<typename U>
//	bool operator!=(const custom_allocator<U>& other) const {
//		return !(*this == other);
//	}

	std::size_t m_numAlloc = 0;
	std::size_t m_numDealloc = 0;
//	pointer m_start;
//	pointer m_end;
};


int main() {
	cout << "TestClass typeid: " << typeid(TestClass).name();
	int deque_size = 0;
	{
		using std_deque = std::deque<TestClass, custom_allocator<TestClass>>;
		std_deque myDeque;
		myDeque.emplace_back(1);
		myDeque.emplace_back(2);
		myDeque.emplace_back(3);
		deque_size = myDeque.size();
	}
	int numConstructsCalled = 0;
	numConstructsCalled = TestClass::s_numConstructsCalled;
	std::cout << "Number of elements within deque with size function: " << deque_size << std::endl;
	std::cout << "Number of elements constructed (counted from inner type): " << numConstructsCalled << std::endl;
	std::cout << "Number of elements destroyed (counted from inner type): " << TestClass::s_numDestructorsCalled << std::endl;
	std::cout << "Number of elements destroyed (count from destroy of custom_allocator): " << actual_elements_destroyed
	          << std::endl;
	return 0;
}
    