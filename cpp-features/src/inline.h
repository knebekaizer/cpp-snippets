//
// Created by vdi on 19.08.24.
//

#pragma once

// inline is extern by default
static int foo(long k) { return k + 1; }
inline int bar(long k) { return k + 1; }
static inline int baz(long k) { return k + 1; }

extern void test_external();