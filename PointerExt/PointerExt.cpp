//! @file Use lower bits of the pointer

#include <iostream>
#include "../include/trace.h"

#include <type_traits>


template<typename T, int numbits>
class PointerExt {
	uintptr_t ptr_ = 0;
public:

	static constexpr uintptr_t bitmask = (1 << numbits) -1;
	static_assert(std::alignment_of<T>() + 2 >= numbits, "Required extension bits extends the maximum for this pointer type");

	PointerExt(T* ptr, uintptr_t bits = 0) : ptr_(reinterpret_cast<uintptr_t>(ptr)) {
		assert(bits <= bitmask);
		ptr_ |= bits;
		TraceX((void*)ptr_);
	}

	uintptr_t bits() const { return ptr_ & bitmask; }

	// should it be explicit?
	operator T*() const { return reinterpret_cast<T*>(ptr_ & ~bitmask); }
};

struct Any { double f; };
struct AnyPacked { double f; } __attribute__((__packed__));

int main() {
	log_info << "Start...";

	{
		auto a = new Any;
		TraceX(a);
		PointerExt<Any, 2> x(a, 1);
		TraceX(static_cast<Any *>(x));
		Any* b = x;
		assert(b == a);
		TraceX(b, x.bits());
	}

	{
		using T = const Any;
		T* a = new Any;
		TraceX(a);
		auto bits = 0;
		// PointerExt<T, 4> x(a, bits = 21); // runtime assert
		PointerExt<T, 4> x(a, bits = 15);
		TraceX(static_cast<T*>(x));
		T* b = x;
		assert(b == a);
		assert(x.bits() == bits);
		// Any* c = x; // compiler error
		TraceX(b, x.bits());
	}

	// The following shall fire static_assert
	// PointerExt<T, 4> x(new AnyPacked, 3); // TestMe

	log_info << "...Done";
	return 0;
}