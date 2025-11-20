//
// Created by vdi on 20.11.25.
//
#include "trace.hpp"
#include <string.h>
// Zero-initialization and default ctor

struct S1 {
	int x;
};

struct S2 {
	S2();
	int x;
};
S2::S2() = default; // x is not zeroed! - does not initialize members

struct S3 : S1 {
	S3() { TraceF; };
	// int x;
};

extern void foo(void*) {}

template <class S>
void test_reinit() {
	log_trace << __PRETTY_FUNCTION__;
	[[maybe_unused]]
	auto a = new S;
	a->x = 1;
	TraceX(a->x);
	// foo(a);
	// memmove((void*)&u, (void*)a, sizeof(S));
	[[maybe_unused]]
	auto b = new(a) S; // default initialization => no init for built-in type / not zeroed
	// TraceX(a->x);
	TraceX(b->x);
	[[maybe_unused]]
	auto c = new(a) S();  // value initialization => built-in zeroed (direct-initialized)
	// TraceX(c->x);
	// foo(c);
	TraceX(c->x);
}

int main() {
	test_reinit<S1>();
	test_reinit<S2>();
	test_reinit<S3>();
	return 0;
}