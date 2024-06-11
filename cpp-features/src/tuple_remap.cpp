#include "trace.hpp"
#include <iostream>
#include <tuple>
#include <cassert>

using namespace std::string_literals;

//#if (__cplusplus < 201700)
#include "cpp14_stubs.hpp"
//#endif

using namespace std;

class Foo;

/**
 * Use the type and value tags as semantic identifiers.
 * Tag may be mapped to anything reasonable, for ex. Float might be actually `double`, or a custom type.
 * for ex: EType::SequentialContainer
*/
enum class EType {Int, Float, Foo, String};
enum class EValue {one, two, random};

/**
 * I want to initialize the variable of type EType with value EValue. The following should be acceptable:
 * \code
 * Foo(1, "abc"); // native ctor
 * Foo(random);
 * Foo(one, two);
 * Foo(tuple{1, "abc"});
 * Foo(tuple{one, two})
 * \endcode
 */


// custom class example
class Foo {
    friend ostream& operator<<(ostream& os, const Foo& foo);
public:
    Foo(int i, const char* s) : i_{i}, s_{s} {}
    Foo(int i, string s) : i_{i}, s_{std::move(s)} {}
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


/*
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
*/
using Types = std::tuple<int, float, Foo, string>;
template <EType t> using
Type = typename std::tuple_element<static_cast<size_t>(t), Types>::type;

/// Converts EValue enum to the parameters of actual type ctor
template <EType t> struct ValueMapper {
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

template<> struct ValueMapper<EType::String> {
	static constexpr auto t = EType::String;
    using T = Type<t>;
    static T args(EValue v) {
        using F = T (*)();
        static F f[] = {
            [] { return "one"s; }, // or []() -> T { return "one"; }
            []{ return "two"s; },
            []{ return "random"s; },
        };
        auto v_ = static_cast<size_t>(v);
        assert(v_ < sizeof(f)/sizeof(*f));
        return f[v_]();
    }
};

template <EType t> struct Value {
	using T = Type<t>;
	Value(EValue v) : v_(ValueMapper<t>::args(v)) {}
	T v_;
	operator T& () { return v_; }
	operator T const& () const { return v_; }
};


template <> struct ValueMapper<EType::Foo> {
    using T = Type<EType::Foo>;
    using Args = tuple<int, string>;
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

	/// Map tuple to another tuple (EValues to real types)
	static Args args(tuple<EValue, EValue> evs) {
		return {ValueMapper<EType::Int>::args(get<0>(evs)), ValueMapper<EType::String>::args(get<1>(evs))};
	}
	static Args args(EValue v1, EValue v2) {
//		return Args{v1, v2}; // Nope
		return Args{Value<EType::Int>(v1), Value<EType::String>(v2)};
//		return {ValueMapper<EType::Int>::args(v1), ValueMapper<EType::String>::args(v2)};
	}
//    static T construct(EValue v) {
//        return cpp14_stubs:: make_from_tuple<T>(args(v));
//    }
};


template <EType t> struct Param {
	using T = Type<t>;
	Param(EValue v)  // NOLINT(google-explicit-constructor)
		: value{ValueMapper<t>::args(v)} {}
    T value;
    operator T& ()  { return value; } // NOLINT(google-explicit-constructor)
    operator const T& () const { return value; } // NOLINT(google-explicit-constructor)

    // ctor "from tuple" allows to construct non-movable type overcoming c++14 issue limitation
    template <class Tuple>
    Param(Tuple&& tup) // NOLINT(google-explicit-constructor)
        : Param(std::forward<Tuple>(tup),
            std::make_index_sequence<std::tuple_size<std::remove_reference_t<Tuple>>::value>{}) {}
private:
    // Helper to unpack tuple
    template <typename Tuple, size_t... Idx>
    constexpr Param(Tuple&& tup, std::index_sequence<Idx...>)
        : value{std::get<Idx>(std::forward<Tuple>(tup))...} {}
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
    Foo foo2 = make_from_tuple<T>(ValueMapper<EType::Foo>::args(EValue::two));
    TraceX(foo2);
}


void test() {
    Param<EType::Int> p1(EValue::random);
    TraceX(p1.value);
    Type<EType::Int> i_random{Param<EType::Int>(EValue::random)};
    TraceX(i_random);
    Type<EType::Int> i1{Param<EType::Int>(EValue::one)};
    TraceX(i1);

    // For c++14 this requires Foo to be MoveConstructible
    test1();

    // This works with c++14 when Foo move ctor deleted
	log_trace << "Works with c++14 even when Foo move ctor deleted:";
    Param<EType::Foo> foo{ValueMapper<EType::Foo>::args(EValue::random)};
    Foo& fr = foo;
    TraceX(fr);
}

int main ()
{
	test();

	auto t = make_tuple(42);
	[[maybe_unused]]
	int x = cpp14_stubs::make_from_tuple<int>(t);
}
