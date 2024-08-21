//
// Created by vdi on 19.08.24.
//
#include "trace.hpp"
#include "inline.h"

inline int foo2(long k) { return k + 1; }

void test_external() {
    TraceX((void*)&foo);
    TraceX((void*)&foo2);
    TraceX((void*)&bar);
    TraceX((void*)&baz);
}