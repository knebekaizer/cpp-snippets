#include <iostream>
#include <type_traits>

using namespace std;


struct A {
	int i = 42;
};

struct B {
	int i = 42;
	ostream& print(ostream& os) const { return os << i; }
};


template<class>
struct sfinae_true : std::true_type{};

namespace details {
template<class T>
static auto has_print(int) -> sfinae_true<decltype(&T::print)>;

template<class>
static auto has_print(long) -> std::false_type;
} // details::

template<class T>
struct is_printable : decltype(details::has_print<T>(0)){};

template<typename T,
		typename std::enable_if<is_printable<T>::value>::type* = nullptr>
ostream& print(ostream& os, const T& x) {
	return x.print(os);
}


template<typename T
		, typename std::enable_if<is_printable<T>::value>::type* = nullptr // OK
	>
ostream& operator<<(std::ostream& os, T const& x) {
	return x.print(os);
}


int main() {
	B b;
	cout << boolalpha;
	b.print(cout) << endl;
	cout << is_printable<B>::value << endl;

	print(cout, b) << endl;
	cout << b << endl;
//	cout << A() << endl;
	return 0;
}
