//
// Created by vdi on 19.08.24.
//
#include "trace.hpp"

#include "inline.h"
using namespace std;

inline int foo2(long k) { return k + 1; }

int main() {
    cout << boolalpha;
    typedef int (*fun)(long);
    fun fp = foo;
    TraceX(&foo == &bar);
    TraceX(&foo);

    TraceX((void*)&foo);
    TraceX((void*)&foo2);
    TraceX((void*)&bar);
    TraceX((void*)&baz);

    TraceX(fp);
    TraceX((void*)fp);
    test_external();
}
