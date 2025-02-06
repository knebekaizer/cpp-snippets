#include "trace.hpp"

#include <array>
#include <algorithm>
#include <iostream>
#include <string>
#include <string_view>

using namespace std;

/**
 * If I don't need the enum names
 * then I can I use the literal as a typed named constant
 */


enum class NoNum {}; // Empty
[[maybe_unused]] constexpr array
#if (__cplusplus < 201700)
    <const char*, 4>    // Use make_array
#endif
    NumS { "none", "one", "two", "three"};


#if (__cplusplus >= 201700)
template <typename T, T... index> constexpr NoNum s2e(const char* str, std::integer_sequence<T, index...> seq) {
	constexpr T invalid = static_cast<T>(-1);
	constexpr auto isValid = [invalid](T x) { return x != invalid; };
	T found = static_cast<T>(-1);

	//  Old fashion: use dummy initializer and fold expression
//	using unused = int[];
//	(void)unused{((found = std::get<index>(NumS) == string_view(str) ? index : found), 0)...}; // ok
	//  Better: use fold expression
	(isValid(found = (std::get<index>(NumS) == std::string_view(str)) ? index : found) || ...);

	return static_cast<NoNum>(found);
}
#else
constexpr int s2e_unfold(const char* str) {
	return -1;
}

template <typename T, class...Ts>
constexpr T s2e_unfold(const char* str, T&& i, Ts&&...as) {
	if (str == NumS[i]) return i;
	return s2e_unfold(str, std::forward<Ts>(as)...);
}

template <typename T, T... index>
constexpr NoNum s2e(const char* str, std::integer_sequence<T, index...> seq) {
	return static_cast<NoNum>(s2e_unfold(str, index...));
}

#endif


constexpr NoNum s2e(const char* str) {
    return s2e(str, std::make_index_sequence<NumS.size()>{});
}

constexpr enum NoNum operator""_f (const char* str, std::size_t) {
    return s2e(str);
}

std::ostream& operator<<(ostream& os, enum NoNum n) {
    auto i = static_cast<size_t>(n);
    return os << (i < NumS.size() ? NumS[i] : "Invalid");
}

template <enum NoNum n>
void bar() {
    TraceX(n);
}



int main() {
//	test();
//	[[maybe_unused]] constexpr auto t = "one"_tag;
//	bar<"one"_tag>();
//	bar<"two"_tag>();
//	bar<"blabla"_tag>();
//
//	TraceX('a'_s);
//	log_trace << 0xa_s;
//	log_trace << 0706_s;
//	log_trace << 123_s;
//	log_trace << 12345678_s;
//
//	TraceX((string{65,66,67}));
//	TraceX((string{'a', 'b', 'c'}));

    bar<static_cast<NoNum>(21)>();
    bar<"one"_f>();
    bar<"two"_f>();
    bar<"blabla"_f>();
	return 0;
}