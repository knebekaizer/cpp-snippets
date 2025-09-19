//
// Created by Vladimir on 2023-10-31.
//

#include "trace.hpp"

#include <utility>
#include <typeinfo>

#include <deque>
#include <set>


using namespace std;

template <typename T1, typename T2>
typename std::enable_if<std::is_same<T1,T2>::value, bool>::type
test(T1 a = {}, T2 b = {}) {
	log_trace << "Not implemented, or template parameter types are not supported: "
				 "T1: " << typeid(T1).name() << "; T2: " << typeid(T2).name();
	return false;
}

template <typename T1, typename T2>
typename std::enable_if<!std::is_same<T1,T2>::value, bool>::type
test(T1 a = {}, T2 b = {}) {
	log_trace << "Types are: " << typeid(T1).name() << " and " << typeid(T2).name();

	pair<T1, T2> p{a, b};
	[[maybe_unused]] auto first = get<T1, T2>(p);
	[[maybe_unused]] auto second = get<T2, T1>(p);
	TraceX(first, second);
	return true;
}

template <typename T1, typename T2, typename std::enable_if<std::is_same<T1,T2>::value, bool>::type = true>
pair<T2, T1> revert_pair(const pair<T1, T2>&) {
	log_trace << "Not implemented, or template parameter types are not supported: "
	             "T1: " << typeid(T1).name() << "; T2: " << typeid(T2).name();
	return {};
}

template <typename T1, typename T2, typename std::enable_if<!std::is_same<T1,T2>::value, bool>::type = true>
pair<T2, T1> revert_pair(const pair<T1, T2>& p) {
	cout << "Types are: " << typeid(T1).name() << " and " << typeid(T2).name() << endl;
	auto first = get<T1, T2>(p);
	auto second = get<T2, T1>(p);
	return {second, first};
}

template <typename T1, typename T2>
void test_revert(const T1& first, const T2& second) {
	auto a = make_pair(first, second);
	auto b = revert_pair(a);
	if (a == revert_pair(b))
		cout << "PASSED";
	else
		cout << "FAILED";
	cout << endl;
}


template <class T, typename = void>
struct Foo;

template <class T>
struct Foo<T, enable_if_t<is_integral<T>::value>> {
	void operator()() { log_trace << "integral type"; }
};

template <class T>
struct Foo<T, enable_if_t<!is_integral<T>::value>> {
	void operator()() { log_trace << "non-integral type"; }
};


enum class E : unsigned {none, one, two, three};
ostream& operator<<(ostream& os, E e) {
	static constexpr const char* a[] = {"none", "one", "two", "three"};
	auto i = underlying_type_t<E>(e);
	assert(i < sizeof(a) / sizeof(*a));
	return os << a[i];
}

template <E e, typename = void>
struct Bar;

template <E e>
struct Bar<e, enable_if_t<e == E::none>> {
	void operator()() { TraceX(1, e); }
};

template <E e>
struct Bar<e, enable_if_t<e != E::none>> {
	void operator()() { TraceX(2, e); }
};

//template <E e, enable_if_t<e == E::none, bool> = true>
//struct Foo {
//	void operator()() { TraceX(1, e); }
//};


/// has_member test: 1st way
template <typename Container>
struct has_push_back {
	// Check if T has push_back member function
	template <typename T>
	static auto test(T* t) -> decltype(t->push_back(std::declval<typename T::value_type>()), std::true_type{});

	// Fallback
	template <typename>
	static std::false_type test(...);

	static constexpr bool value = decltype(test<Container>(nullptr))::value;
};
template <typename T>
constexpr bool has_push_back_v = has_push_back<T>::value;


/// has_member test: 2nd way

namespace details {
template<class>
struct sfinae_true : std::true_type{};

template<class T>
static auto has_print(int) -> sfinae_true<decltype(&T::print)>;

template<class>
static auto has_print(long) -> std::false_type;
} // details::

template<class T>
struct is_printable : decltype(details::has_print<T>(0)){};


template<typename T, typename std::enable_if<is_printable<T>::value>::type* = nullptr>
ostream& operator<<(std::ostream& os, T const& x) {
	return x.print(os);
}

// --------------

template <typename T, typename ...Args> class Combo {};

template <typename T, typename ...Args>
constexpr auto getLastParamType(Combo<T, Args...>* c) {
	using Tuple = tuple<Args...>;
//	if constexpr (std::tuple_size_v<Tuple> > 0) {
		using Last = typename std::tuple_element<std::tuple_size_v<Tuple> - 1, Tuple>::type;
		return (Last*)nullptr;
//	} else {
//		return (T*)nullptr;
//	}
}

template <typename Combo>
struct LastParam  {
	using type = decltype(*getLastParamType((Combo*)nullptr));
};

template <typename First, class Combo1, class Combo2, typename std::enable_if<
		!std::is_same<typename LastParam<Combo1>::type, First>::value, bool>::type = true>
auto add(const Combo1& c1, const Combo2& c2) {
	TraceX(typeid(typename LastParam<Combo1>::type).name(), typeid(First).name());
	bool isSame = std::is_same<typename LastParam<Combo1>::type, First>::value;
	TraceX(isSame);
	return true; // TODO
}

template <class Combo1, typename First, typename ...Args>
auto operator+(const Combo1& a, const Combo<First, Args...>& b) {
	return add<First>(a, b);
}

void testCombo() {
	class P {}; // Pancake;
	class C {}; // Caviar;

	Combo<P,C,P> c1;
	Combo<C, P> c2;
	[[maybe_unused]] auto x1 = c1 + c2; // ok
//	Combo<P>{} + Combo<C>{};
	[[maybe_unused]] Combo<P, C> c3;
	[[maybe_unused]] auto x2 = c1 + c3; // Fails, as expected
//	Combo<P,P>{} + Combo<C,C>{}; // ok
//	[[maybe_unused]] auto x4 = Combo<P,C>{} + Combo<C,P>{}; // ok
}

// ----------------------


int main() {
	test<int,long>();
	test<int,int>();

	char s1[] = "foo";
	test<const char*, char*>("bar", s1);

	const signed char s2[] = "foo";
	test<const char*, const signed char*>("bar", s2);

	test_revert(1, 2l);
	test_revert(1, 2);

	static_assert(has_push_back<deque<int>>::value, "Has no push_back");
	static_assert(!has_push_back<set<int>>::value, "Has push_back");

	struct A{};
	struct B{ ostream& print(ostream& os) const { return os << this; } };
	static_assert(!is_printable<A>::value, "Printable check");
	static_assert(is_printable<B>::value, "Printable check");
	log_trace << "B is printable: " << B();

	Foo<int>()();
	Foo<float>()();

	Bar<E::none>()();
	Bar<E::one>()();

	testCombo();
	return 0;
}