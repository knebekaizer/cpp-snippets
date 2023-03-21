#include <vector>
#include <memory>
#include <iostream>
//using namespace std;

namespace fault_injection {

enum class Err : unsigned {NONE, DIFF_ERROR, INCR_MAY_THROW, DIFF_MAY_THROW} err_;
std::ostream& operator<<(std::ostream& os, Err err) {
	constexpr static const char* names[] = {"NONE", "DIFF_ERROR", "INCR_MAY_THROW", "DIFF_MAY_THROW"};
	return os << names[static_cast<unsigned>(err)];
}

template <class Tp_> struct Ptr;

template <class T>
struct Ptr {
    using P = T*;
    T* p_;
    Ptr(nullptr_t) : p_(nullptr) {}
    Ptr(T* p) : p_(p) {}
    Ptr(const T*& p) : p_(p) {}
    Ptr(const Ptr<T>&) = default;
    Ptr(Ptr<T>&&) = default;
    Ptr& operator=(const Ptr<T>&) = default;
    Ptr& operator=(Ptr<T>&&) = default;

    T& operator[](size_t n) { return *(p_ + n); }
    const T& operator[](size_t n) const { return *(p_ + n); }

    template <class U>
    Ptr(Ptr<U> other) : Ptr(static_cast<T*>(other.p_)) {}
    T* operator->() { return p_; }
    T* operator->() const { return p_; }
    bool operator==(const Ptr<T> other) const { return p_ == other.p_; }
    bool operator==(nullptr_t) const { return p_ == nullptr; }
    bool operator!=(const Ptr<T> other) const { return !operator==(other); }
    bool operator!=(nullptr_t) const { return p_ != nullptr; }
    Ptr& operator++() {++p_; return *this;}
    Ptr  operator++(int) { Ptr t(*this); ++p_; return t;}
    Ptr& operator--() { --p_; return *this;}
    Ptr  operator--(int) { Ptr t(*this); --p_; return t; }

    Ptr operator+(size_t n) {
        if (err_ == Err::INCR_MAY_THROW) throw std::logic_error("logic_error #11");
        return Ptr(p_ + n);
    }
    Ptr operator-(size_t n) { return Ptr(p_ - n); }
};


template <class T>
ptrdiff_t operator-(const Ptr<T> a, const Ptr<T>& b) {
	switch (err_) {
		case Err::DIFF_MAY_THROW:
			throw std::logic_error("logic_error #12");
		case Err::DIFF_ERROR:
			return 1 + a.p_ - b.p_;
		default:
			return a.p_ - b.p_;
	}
}

template <class T>
struct CPtr {
    const T* p_;
    CPtr(T* p) : p_(p) {}
    CPtr(Ptr<T> ptr) : CPtr(ptr.p_) {}
    template <class U>
    CPtr(CPtr<U> other) : CPtr(static_cast<const T*>(other.p_)) {}
    const T* operator->() const { return p_; }
};

template <class Tp_> class alloc;

template <>
class alloc<void>
{
public:
    typedef void*             pointer;
    typedef const void*       const_pointer;
    typedef void              value_type;
    template <class Up_> struct rebind {typedef alloc<Up_> other;};
};

template<class Tp_>
class alloc {
public:
    typedef size_t size_type;
    typedef ptrdiff_t difference_type;
    typedef Ptr<Tp_> pointer; // typedef Tp_ *pointer;
    typedef const Ptr<Tp_> const_pointer; // typedef const Tp_ *const_pointer;
    typedef Tp_ &reference;
    typedef const Tp_ &const_reference;
    typedef Tp_ value_type;

    typedef std::true_type propagate_on_container_move_assignment;
    typedef std::true_type is_always_equal;

    template<class Up_>
    struct rebind {
        typedef alloc<Up_> other;
    };

    alloc() noexcept {}

    template<class _Up>
    alloc(const alloc<_Up> &) noexcept {}

     pointer address(reference __x) const noexcept { return std::addressof(__x); }

     const_pointer address(const_reference __x) const noexcept {
        return std::addressof(__x);
    }

    pointer allocate(size_type n, alloc<void>::const_pointer = 0) {
        if (n > max_size())
            std::__throw_length_error("allocator<T>::allocate(size_t n)"
                                 " 'n' exceeds maximum supported size");
        return static_cast<Tp_*>(std::__libcpp_allocate(n * sizeof(Tp_), alignof(Tp_)));
    }

     void deallocate(pointer p, size_type n) noexcept {
        std::__libcpp_deallocate((void *) p.p_, n * sizeof(Tp_), alignof(Tp_));
    }

     size_type max_size() const noexcept { return size_type(~0) / sizeof(Tp_); }

    template<class _Up, class... _Args>
    void construct(_Up *__p, _Args &&... __args) {
        ::new((void *) __p) _Up(std::forward<_Args>(__args)...);
    }

     void destroy(pointer p) { p->~Tp_(); }
};

} // namespace fault_injection

namespace fi = fault_injection;
using namespace std;

void test(fi::Err err) {
	fi::err_ = err;
    cout << "err = " << err << endl;
    try {
        vector<int, fi::alloc<int>> v(10);
	    auto sz = v.size();
        cout << "size = " << sz << " : expected 10" <<  endl;
    }
    catch (std::exception& e) {
        cout << "Exception handled " << e.what() << endl;
    }
    catch (...) {
        cout << "Exception handled " << endl;
    }
}

int main() {
    test(fi::Err::NONE);
    test(fi::Err::DIFF_ERROR);
    test(fi::Err::INCR_MAY_THROW);
    test(fi::Err::DIFF_MAY_THROW);
	return 0;
}

/*
Output:
err = NONE
size = 10 : expected 10
err = DIFF_ERROR
size = 11 : expected 10
err = INCR_MAY_THROW
Exception handled logic_error #11
err = DIFF_MAY_THROW
libc++abi.dylib: terminating with uncaught exception of type std::logic_error: logic_error #12
*/