#include "trace.hpp"
#include <iostream>
#include <tuple>
#include <functional>

using namespace std;

struct Foo {
    Foo(int i, string s) { TraceX(i, s); }
    Foo(const Foo&) = delete; // apply by the pointer; no copy
    Foo(Foo&&) = delete;  // check copy elision
    Foo& operator=(const Foo&) = delete;
    Foo& operator=(Foo&&) = delete;
    static void callStatic(int i, string s) { TraceX(i, s); }
    void callDyn(int i, string s) { TraceX(i, s); }
};

void test_apply(){
    tuple<int, string> param{11, "onetwo"s};
    std::apply(Foo::callStatic, param);
    Foo foo(99, "three");
    std::apply(std::mem_fn(&Foo::callDyn), make_tuple(&foo, 11, "onetwo"s));
    auto s2 = make_from_tuple<Foo>(param);
    std::apply(std::mem_fn(&Foo::callDyn), make_tuple(&s2, 11, "onetwo"s));

//    std::apply(std::mem_fn(&Str::Str), make_tuple(str, 11, "onetwo"s));
//    std::apply(Str::Str, param);
}

using Param = tuple<int, string>;
class Env {
public:
    Env(Param&& param) : obj_(make_from_tuple<Foo>(param)) {}
    Foo obj_;
};

void testEnv() {
    tuple<int, string> param{11, "onetwo"s};
    Env env(std::move(param));
}

int main ()
{
    test_apply();
    testEnv();
}
