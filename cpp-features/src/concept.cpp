//! \file

#include <iostream>
#include <type_traits>
#include <concepts>

#include "trace.hpp"

using namespace std;
//struct ThreadPool {
//  void enqueue( std::invocable auto fun );
//};

template <typename T>
concept Printable = requires(T t, std::ostream& os) {
    { t.print(os) } -> std::convertible_to<std::ostream&>; //std::ostream&;
};

struct P {
    ostream& print(ostream& os) const { return os << "I am printable!"; };
};

template <Printable P>
std::ostream& operator<<(std::ostream& os, P const& p) {
    return p.print(os);
}

template <typename T>
concept HasFoo = requires(T t, int i) {
    { t.foo(i) } -> std::convertible_to<int>;
};

struct Foo {
    int foo() const {  return 5; }
};

std::ostream& operator<<(std::ostream& os, Foo const& p) {
    return os << "Foo: " << p.foo();
}

struct FooD : Foo {
};

struct Bar
{
    int foo(unsigned) {
        return 5;
    }
};

int main()
{
    cout << "Foo: ";
    if constexpr (HasFoo<Foo>)
        cout << "yes";
    else
        cout << "no";
    cout << endl;

    cout << "Bar: ";
    if constexpr (HasFoo<Bar>)
        cout << "yes";
    else
        cout << "no";
    cout << endl;

    cout << "P is Printable: ";
    if constexpr (Printable<P>)
        cout << "yes";
    else
        cout << "no";
    cout << endl;

    TraceX(P());
    TraceX(Foo());
    TraceX(FooD());
}
