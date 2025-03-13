//
// Created by Vladimir on 2024-06-12.
//

#include <tuple>
#include <iostream>

using namespace std;

# pragma mark ## split
namespace detail
{
template <std::size_t Ofst, class Tuple, std::size_t... I>
constexpr auto slice_impl(Tuple&& t, std::index_sequence<I...>)
{
	return std::forward_as_tuple(
			std::get<I + Ofst>(std::forward<Tuple>(t))...);
}
}

template <std::size_t I1, std::size_t I2, class Cont>
constexpr auto tuple_slice(Cont&& t)
{
	static_assert(I2 >= I1, "invalid slice");
	static_assert(std::tuple_size<std::decay_t<Cont>>::value >= I2,
	              "slice index out of bounds");

	return detail::slice_impl<I1>(std::forward<Cont>(t),
	                              std::make_index_sequence<I2 - I1>{});
}

// Usage:
// tuple_slice<I1, I2>(t);

namespace details {
template<class... Ts>
ostream& printAll(ostream& os, Ts&& ... args) {
	//	f(&args...); // “&args...” is a pack expansion
	// “&args” is its pattern
	auto sep = "";
	([&] {        // Do things in your "loop" lambda
		os << sep << args;
		sep = ", ";
	}(), ...);
	return os;
}

template<class Tuple, std::size_t... I>
constexpr ostream& printTuple(ostream& os, Tuple&& t, std::index_sequence<I...>) {
	os << '{';
	printAll(os, std::get<I>(std::forward<Tuple>(t))...);
	os << '}';
	return os;
}
}

template <typename ...Args>
ostream& operator<<(ostream& os, const tuple<Args...>&& tup) {
	using T = const tuple<Args...>;
	auto constexpr size = std::tuple_size<std::decay_t<T>>::value;
	return details::printTuple(os, tup, std::make_index_sequence<size>{});
}

template <typename T, typename ...Args> class Combo;
class Pancake;

template <typename T, typename ...Args>
auto LastParamType(Combo<T, Args...>&& c) {
	using Tuple = tuple<Args...>;
	using Last = typename std::tuple_element<std::tuple_size_v<Tuple> - 1, Tuple>::type;
	return is_same<Last, Pancake>::value;
}

template <class Combo1, class Combo2, typename std::enable_if<!std::is_same<LastParamType(Combo1),LastParamType<Combo2>>::value, bool>::type = true>
auto add(Combo1&& c1, Combo c2) {

}

template <class Combo1, typename T, typename ...Args>
auto operator+(Combo1&& c1, Combo<T, Args...>&& c2) {
	return add(c1, c2);
}

int main() {
//	printAll(1,2.0, "wertyu");
	cout << "Tuple: " << make_tuple(1,2.0, "wertyu") << endl;
	cout << "Done." << endl;
	return 0;
}