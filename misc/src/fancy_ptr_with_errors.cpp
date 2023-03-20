
//#include "fancy_ptr.h"

#include <vector>
#include <memory>
#include <iostream>
using namespace std;

// -----------------------------------------------------------------------
#define ERROR_  1ul
static auto err_ = 0;
// -----------------------------------------------------------------------

template <class Tp_> class Ptr;

template <class T>
struct Ptr {
//    using T = int;
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
#ifdef ERROR_
        if (err_ == 11) throw std::logic_error("logic_error #11");
#endif
        return Ptr(p_ + n);
    }
    Ptr operator-(size_t n) { return Ptr(p_ - n); }

};


template <class T>
ptrdiff_t operator-(const Ptr<T> a, const Ptr<T>& b) {
#ifdef ERROR_
    if (err_ == 12) throw std::logic_error("logic_error #12");
    return err_ + a.p_ - b.p_;
#endif
    return a.p_ - b.p_;
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
class _LIBCPP_TEMPLATE_VIS alloc<void>
{
public:
    typedef void*             pointer;
    typedef const void*       const_pointer;
    typedef void              value_type;

    template <class Up_> struct rebind {typedef alloc<Up_> other;};
};

template<class Tp_>
class _LIBCPP_TEMPLATE_VIS alloc {
public:
    typedef size_t size_type;
    typedef ptrdiff_t difference_type;
    typedef Ptr<Tp_> pointer; // typedef Tp_ *pointer;
//    typedef const CPtr<Tp_> const_pointer; // typedef const Tp_ *const_pointer;
    typedef const Ptr<Tp_> const_pointer; // typedef const Tp_ *const_pointer;
    typedef Tp_ &reference;
    typedef const Tp_ &const_reference;
    typedef Tp_ value_type;

    typedef true_type propagate_on_container_move_assignment;
    typedef true_type is_always_equal;

//    template<class Up_>
//    struct rebind {
//        typedef alloc<Up_> other;
//    };

    _LIBCPP_INLINE_VISIBILITY _LIBCPP_CONSTEXPR_AFTER_CXX17
    alloc() _NOEXCEPT {}

    template<class _Up>
    _LIBCPP_INLINE_VISIBILITY _LIBCPP_CONSTEXPR_AFTER_CXX17
    alloc(const alloc<_Up> &) _NOEXCEPT {}

    _LIBCPP_INLINE_VISIBILITY pointer address(reference __x) const _NOEXCEPT { return _VSTD::addressof(__x); }

    _LIBCPP_INLINE_VISIBILITY const_pointer address(const_reference __x) const _NOEXCEPT {
        return _VSTD::addressof(__x);
    }

    _LIBCPP_NODISCARD_AFTER_CXX17 _LIBCPP_INLINE_VISIBILITY
    pointer allocate(size_type n, alloc<void>::const_pointer = 0) {
        if (n > max_size())
            __throw_length_error("allocator<T>::allocate(size_t n)"
                                 " 'n' exceeds maximum supported size");
//        return static_cast<pointer>(_VSTD::__libcpp_allocate(n * sizeof(Tp_), _LIBCPP_ALIGNOF(Tp_)));
        return static_cast<Tp_*>(_VSTD::__libcpp_allocate(n * sizeof(Tp_), _LIBCPP_ALIGNOF(Tp_)));
    }

    _LIBCPP_INLINE_VISIBILITY void deallocate(pointer p, size_type n) _NOEXCEPT {
        _VSTD::__libcpp_deallocate((void *) p.p_, n * sizeof(Tp_), _LIBCPP_ALIGNOF(Tp_));
    }

    _LIBCPP_INLINE_VISIBILITY size_type max_size() const _NOEXCEPT { return size_type(~0) / sizeof(Tp_); }

    template<class _Up, class... _Args>
    _LIBCPP_INLINE_VISIBILITY
    void
    construct(_Up *__p, _Args &&... __args) {
        ::new((void *) __p) _Up(_VSTD::forward<_Args>(__args)...);
    }

    _LIBCPP_INLINE_VISIBILITY void destroy(pointer p) { p->~Tp_(); }
};

void test(int err = 0) {
    err_ = err;
    cout << "err_ = " << err_ << endl;
    try {
        vector<int, alloc<int>> v(10);
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
    test(0);
    test(1);
    test(11);
    test(12);
    return 0;
}

/*
Output:
err_ = 0
size = 10 : expected 10
err_ = 1
size = 11 : expected 10
err_ = 11
Exception handled logic_error #11
err_ = 12
libc++abi.dylib: terminating with uncaught exception of type std::logic_error: logic_error #12
 */