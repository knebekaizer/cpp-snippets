/*******************************************************************************
 * Copyright (c) 2022 .
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 *******************************************************************************/

#ifndef FANCY_PTR_H
#define FANCY_PTR_H


#include <vector>
#include <memory>

using std::nullptr_t;
using std::true_type;

#include <string>
#include <exception>

std::string Backtrace(int skip = 1);

namespace my {

class Error : public std::runtime_error {
public:
	Error(const std::string& s) : std::runtime_error(s) {}
};

//inline void throw_my() { throw Error(Backtrace(2)); }

template<class Tp> class Ptr;
template<class Tp> class CPtr;

template<class T>
struct Ptr {
	typedef ptrdiff_t difference_type; //almost always ptrdiff_t
	typedef T value_type; //almost always T
	typedef T& reference; //almost always T& or const T&
	typedef const T& const_reference; //almost always T& or const T&
	typedef T* pointer; //almost always T* or const T*
	typedef const T* const_pointer; //almost always T* or const T*
	typedef std::random_access_iterator_tag iterator_category;

	using P = T*;
	T* p_;

	Ptr() : p_(nullptr) {}
	Ptr(nullptr_t) : p_(nullptr) {}
	Ptr(T* p) : p_(p) {}

//	Ptr(const T* p) : p_(const_cast<T*>(p)) {}    //! FIXME

	Ptr(const Ptr<T>&) = default;
	Ptr(Ptr<T>&&) = default;
	Ptr& operator=(const Ptr<T>&) = default;
	Ptr& operator=(Ptr<T>&&) noexcept = default;

	T& operator[](size_t n) { return *(p_ + n); }
	const T& operator[](size_t n) const { return *(p_ + n); }

	template<class U>
	Ptr(Ptr<U> other) : Ptr(static_cast<T*>(other.p_)) {}

	T* operator->() { return p_; }
//	const T* operator->() const { return p_; }
	T* operator->() const { return p_; }

	T& operator*() { return *p_; }
	T& operator*() const { return *p_; }

	bool operator==(const Ptr<T>& other) const { return p_ == other.p_; }
	bool operator==(const CPtr<T>& other) const { return p_ == other.p_; }
	bool operator==(nullptr_t) const { return p_ == nullptr; }

	bool operator!=(const Ptr<T>& other) const { return !operator==(other); }
	bool operator!=(const CPtr<T>& other) const { return !operator==(other); }
	bool operator!=(nullptr_t) const { return p_ != nullptr; }

	bool operator<(const Ptr<T>& other) const { return p_ < other.p_; }
	bool operator>(const Ptr<T>& other) const { return p_ > other.p_; }
	bool operator<=(const Ptr<T>& other) const { return p_ <= other.p_; }
	bool operator>=(const Ptr<T>& other) const { return p_ >= other.p_; }

	explicit operator bool() const { return operator!=(nullptr); }

	Ptr& operator++() { ++p_; return *this; }
	Ptr operator++(int) { Ptr t(*this); ++p_; return t; }
	Ptr& operator--() { --p_; return *this; }
	Ptr operator--(int) { Ptr t(*this); --p_; return t; }

	Ptr operator+(size_t n) const { return Ptr(p_ + n); }
	Ptr operator-(size_t n) const { return Ptr(p_ - n); }
	Ptr operator+=(size_t n) const { return *this + n; }
	Ptr operator-=(size_t n) const { return *this - n; }
};

template<class T>
struct CPtr {
	typedef ptrdiff_t difference_type; //almost always ptrdiff_t
	typedef T value_type; //almost always T
//	typedef T& reference; //almost always T& or const T&
	typedef const T& const_reference; //almost always T& or const T&
//	typedef T* pointer; //almost always T* or const T*
	typedef const T* const_pointer; //almost always T* or const T*
	typedef std::random_access_iterator_tag iterator_category;

	const T* p_;

	CPtr() : p_(nullptr) {}
	CPtr(nullptr_t) : p_(nullptr) {}
	CPtr(const T* p) : p_(p) {}

	CPtr(const Ptr<T>& p) : p_(p.p_) {}

	CPtr(const CPtr<T>&) = default;
	CPtr(CPtr<T>&&) = default;
	CPtr& operator=(const CPtr<T>&) = default;
	CPtr& operator=(CPtr<T>&&) noexcept = default;

//	T& operator[](size_t n) { return *(p_ + n); }
	const T& operator[](size_t n) const { return *(p_ + n); }

	template<class U>
	CPtr(CPtr<U> other) : CPtr(static_cast<const T*>(other.p_)) {}

	const T* operator->() { return p_; }
	const T* operator->() const { return p_; }

	const T& operator*() { return *p_; }
	const T& operator*() const { return *p_; }

	bool operator==(const CPtr<T>& other) const { return p_ == other.p_; }
	bool operator==(const Ptr<T>& other) const { return p_ == other.p_; }
	bool operator==(nullptr_t) const { return p_ == nullptr; }

	bool operator!=(const CPtr<T>& other) const { return !operator==(other); }
	bool operator!=(const Ptr<T>& other) const { return !operator==(other); }
	bool operator!=(nullptr_t) const { return p_ != nullptr; }

	bool operator<(const CPtr<T>& other) const { return p_ < other.p_; }
	bool operator>(const CPtr<T>& other) const { return p_ > other.p_; }
	bool operator<=(const CPtr<T>& other) const { return p_ <= other.p_; }
	bool operator>=(const CPtr<T>& other) const { return p_ >= other.p_; }

	explicit operator bool() const { return operator!=(nullptr); }

	CPtr& operator++() { ++p_; return *this; }
	CPtr operator++(int) { CPtr t(*this); ++p_; return t; }
	CPtr& operator--() { --p_; return *this; }
	CPtr operator--(int) { CPtr t(*this); --p_; return t; }

	CPtr operator+(size_t n) const { return CPtr(p_ + n); }
	CPtr operator-(size_t n) const { return CPtr(p_ - n); }
	CPtr operator+=(size_t n) const { return *this + n; }
	CPtr operator-=(size_t n) const { return *this - n; }

	difference_type operator-(const CPtr& p) const { return (p_ - p.p_); }
};

template<typename T>
bool operator<(const Ptr<T>& a, const CPtr<T>& b) { return a.p_ < b.p_; }

template<typename T>
bool operator<(const CPtr<T>& a, const Ptr<T>& b) { return a.p_ < b.p_; }

template<typename T>
bool operator<=(const Ptr<T>& a, const CPtr<T>& b) { return a.p_ <= b.p_; }

template<typename T>
bool operator<=(const CPtr<T>& a, const Ptr<T>& b) { return a.p_ <= b.p_; }

template<class T>
ptrdiff_t operator-(const Ptr<T>& a, const Ptr<T> b) {
	return a.p_ - b.p_;
}


template<class Tp>
class custom_allocator;

template<>
class custom_allocator<void> {
public:
	typedef void* pointer;
	typedef const void* const_pointer;
	typedef void value_type;

	template<class Up>
	struct rebind {
		typedef custom_allocator<Up> other;
	};
};

template<class Tp>
class custom_allocator {
public:
	typedef size_t size_type;
	typedef ptrdiff_t difference_type;
	typedef Ptr<Tp> pointer; // was: typedef Tp *pointer;
	typedef CPtr<Tp> const_pointer; // typedef const Tp *const_pointer;
	typedef Tp& reference;
	typedef Tp const& const_reference;
	typedef Tp value_type;

	typedef true_type propagate_on_container_move_assignment;
	typedef true_type is_always_equal;

	custom_allocator() noexcept {}

	template<class _Up>
	custom_allocator(const custom_allocator<_Up>&) noexcept {}

//    pointer address(reference __x) const noexcept { return std::addressof(__x); }
//
//    const_pointer address(const_reference __x) const noexcept {
//        return std::addressof(__x);
//    }

	pointer allocate(size_type n, custom_allocator<void>::const_pointer = 0) {
		if (n > max_size())
			std::__throw_length_error("custom_allocatorator<T>::allocate(size_t n)"
			                          " 'n' exceeds maximum supported size");
		return static_cast<Tp*>(std::__libcpp_allocate(n * sizeof(Tp), alignof(Tp)));
	}

	void deallocate(pointer p, size_type n) noexcept {
		std::__libcpp_deallocate((void*) p.p_, n * sizeof(Tp), alignof(Tp));
	}

	size_type max_size() const noexcept { return size_type(~0) / sizeof(Tp); }

	template<class Up, class... Args>
	void
	construct(Up* p, Args&& ... args) {
		::new((void*) p) Up(std::forward<Args>(args)...);
	}

	void destroy(pointer p) { p->~Tp(); }

	bool operator==(const custom_allocator<Tp> other) const { return true; }
	bool operator!=(const custom_allocator<Tp> other) const { return !operator==(other); }

	template<class Up>
	struct rebind {
		typedef custom_allocator<Up> other;
	};

};

} // namespace my

// --------------------------------------------------------------------------------------

template<class T>
struct std::pointer_traits<my::Ptr<T>> {
	typedef my::Ptr<T> pointer;
	using element_type = T;
	using difference_type = std::ptrdiff_t;

	template<class _Up> using rebind = typename std::__pointer_traits_rebind<pointer, _Up>::type;
};

template<class T>
struct std::pointer_traits<const my::Ptr<T>> {
	typedef const my::Ptr<T> pointer;
	using element_type = const T;
	using difference_type = std::ptrdiff_t;

	template<class _Up> using rebind = typename std::__pointer_traits_rebind<pointer, _Up>::type;
	static pointer pointer_to(element_type& __r) { return &__r; }
};

template<class T>
struct std::pointer_traits<my::CPtr<T>> {
	typedef my::CPtr<T> pointer;
	using element_type = const T;
	using difference_type = std::ptrdiff_t;

	template<class _Up> using rebind = typename std::__pointer_traits_rebind<pointer, _Up>::type;
	static pointer pointer_to(element_type& __r) { return &__r; }
};

template<class T>
struct std::pointer_traits<const my::CPtr<T>> {
	typedef const my::CPtr<T> pointer;
	using element_type = const T;
	using difference_type = std::ptrdiff_t;

	template<class _Up> using rebind = typename std::__pointer_traits_rebind<pointer, _Up>::type;
	static pointer pointer_to(element_type& __r) { return &__r; }
};

// ------------------------------------------------------

template<class T>
struct std::iterator_traits<my::Ptr<T>> {
	typedef ptrdiff_t difference_type;
	typedef typename remove_cv<T>::type value_type;
	typedef my::Ptr<T> pointer;
//typedef my::CPtr<T> const_pointer;
	typedef T& reference;
	typedef random_access_iterator_tag iterator_category;
#if _LIBCPP_STD_VER > 17
	typedef contiguous_iterator_tag    iterator_concept;
#endif
};


template<class T>
struct std::iterator_traits<const my::Ptr<T>> {
	typedef ptrdiff_t difference_type;
	typedef typename remove_cv<T>::type value_type;
	typedef const my::Ptr<T> pointer;
	typedef T& reference;
	typedef random_access_iterator_tag iterator_category;
#if _LIBCPP_STD_VER > 17
	typedef contiguous_iterator_tag    iterator_concept;
#endif
};

template<class T>
struct std::iterator_traits<my::CPtr<T>> {
	typedef ptrdiff_t difference_type;
	typedef typename remove_cv<T>::type value_type;
	typedef my::CPtr<T> pointer;
	typedef const my::CPtr<T> const_pointer;
	typedef T& reference;
	typedef const T& const_reference;
	typedef random_access_iterator_tag iterator_category;
#if _LIBCPP_STD_VER > 17
	typedef contiguous_iterator_tag    iterator_concept;
#endif
};

template<class T>
struct std::iterator_traits<const my::CPtr<T>> {
	typedef ptrdiff_t difference_type;
	typedef typename remove_cv<T>::type value_type;
	typedef my::CPtr<T> pointer;
	typedef const my::CPtr<T> const_pointer;
	typedef const T& const_reference;
	typedef random_access_iterator_tag iterator_category;
#if _LIBCPP_STD_VER > 17
	typedef contiguous_iterator_tag    iterator_concept;
#endif
};

#endif //FANCY_PTR_H
