#include "trace.hpp"
#include <iostream>
#include <tuple>
#include <cassert>

using namespace std::string_literals;

//#if (__cplusplus < 201700)
#include "cpp14_stubs.hpp"
//#endif

using namespace std;


// custom class
class Foo {
    friend ostream& operator<<(ostream& os, const Foo& foo);
public:
    Foo(int i, const char* s) : i_{i}, s_{s} {}
    Foo(const Foo&) = delete;
    Foo(Foo&&) = delete; // C++17 only
//    Foo(Foo&&) { assert(("Move ctor shall not be used"s, false)); } // c++14 requires move ctor even with copy elision
private:
    int i_;
    string s_;
};
ostream& operator<<(ostream& os, const Foo& foo) {
    return os << "{" << foo.i_ << ", "  << foo.s_ << "}";
}

enum class EType {Int, Float, Foo};
enum class EValue {one, two, random};

namespace details {
template <typename E, typename std::enable_if<std::is_enum<E>::value, bool>::type = true>
struct TypeList {
    using type = void; // std::tuple<int, float, Foo>;
    static constexpr bool isSupported{false};
};

template <>
struct TypeList<EType> {
    using type = std::tuple<int, float, Foo>;
    static constexpr bool isSupported{true};
};

}


template <EType t>
using Type = // details::TypeList<EType>::type
    typename std::tuple_element<static_cast<size_t>(t), details::TypeList<EType>::type>::type;

using ttt = Type<EType::Int>;

//template <EType t, EValue v> struct ValueMapper;
//template <EType t> struct ValueMapper<t, EValue::one> {
//    Type<t> value{1};
//    operator Type<t> () const { return value; }
//};

/// Converts EValue enum to the parameters of actual type ctor
template <EType t, class E> struct ValueMapper;

template <EType t> struct ValueMapper<t, EValue> {
    // fundamental types only?
    using T = Type<t>;
    static T args(EValue v) {
        using F = T (*)();
        static F f[] = {
            []{ return 1; },
            []{ return 2; },
            []{ return std::rand(); },
        };
        auto v_ = static_cast<size_t>(v);
        assert(v_ < sizeof(f)/sizeof(*f));
        return f[v_]();
    }
};


template <> struct ValueMapper<EType::Foo, EValue> {
    using T = Type<EType::Foo>;
    using Args = tuple<int, const char*>;
    static Args args(EValue v) {
        using F = Args (*)();
        static F f[] = {
                []{ return Args{1, "one"}; },
                []{ return Args{2, "two"}; },
                []{ return Args{std::rand(), "random"}; },
            };
        auto v_ = static_cast<size_t>(v);
        assert(v_ < sizeof(f)/sizeof(*f));
        return f[v_]();
    }
//    static T construct(EValue v) {
//        return cpp14_stubs:: make_from_tuple<T>(args(v));
//    }
};


template <EType t, class E> struct Param {
    using T = Type<t>;
    Param(E v) : value{ValueMapper<t,E>::args(v)} {}
    T value;
    operator T& ()  { return value; }
    operator const T& () const { return value; }

    // ctor "from tuple" allows to construct non-movable type overcoming c++14 issue limitation
    template <class Tuple>
    Param(Tuple&& tup)
        : Param(std::forward<Tuple>(tup),
            std::make_index_sequence<std::tuple_size<std::remove_reference_t<Tuple>>::value>{}) {}
private:
    template <typename _Tuple, size_t... _Idx>
    constexpr Param(_Tuple&& __t, std::index_sequence<_Idx...>)
        : value{std::get<_Idx>(std::forward<_Tuple>(__t))...} {}
};


template <class T = Foo>
void test1(typename std::enable_if<!std::is_move_constructible<T>::value, bool>::type = true) {
    log_trace << "error: use of deleted move constructor ‘Foo::Foo(Foo&&)'";
}

template <class T = Foo>
void test1(typename std::enable_if<std::is_move_constructible<T> ::value, bool>::type = true) {
    tuple<int, const char*> pp{1, "one"};
    Foo foo1 = make_from_tuple<T>(pp); // c++14 only: error: use of deleted function ‘Foo::Foo(Foo&&)’
    TraceX(foo1);
    Foo foo2 = make_from_tuple<T>(ValueMapper<EType::Foo, EValue>::args(EValue::two));
    TraceX(foo2);
}


void test() {
    Param<EType::Int, EValue> p1(EValue::random);
    TraceX(p1.value);
    Type<EType::Int> i_random{Param<EType::Int, EValue>(EValue::random)};
    TraceX(i_random);
    Type<EType::Int> i1{Param<EType::Int, EValue>(EValue::one)};
    TraceX(i1);

    // For c++14 this requires Foo to be MoveConstructible
    test1();

    // This works for c++14 when Foo move ctor deleted
    Param<EType::Foo, EValue> foo{ValueMapper<EType::Foo, EValue>::args(EValue::random)};
    Foo& fr = foo;
    TraceX(fr);
}

int main ()
{
	test();
}
