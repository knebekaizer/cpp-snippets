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
template <typename Str>
constexpr size_t hashString(const Str& toHash)
{
	static_assert(sizeof(size_t) == 8, "FNV algorithm requires 64-bit size_t");
	// FNV-1a 64 bit algorithm
	size_t result = 0xcbf29ce484222325; // FNV offset basis
	for (auto const* p = toHash; *p; ++p  ) { const char c = *p;
//	for (char c : toHash) {
		result ^= c;
		result *= 1099511628211; // FNV prime
	}
	return result;
}

constexpr int s2e_unfold(const char str[]) {
	return -1;
}

template <typename T, class...Ts>
constexpr T s2e_unfold(const char* str, T&& i, Ts&&...as) {
//	if (str == NumS[i]) return i;
	if (hashString(str) == hashString(NumS[i])) return i;
	return s2e_unfold(str, std::forward<Ts>(as)...);
}

template <typename Str, typename T, T... index>
constexpr NoNum s2e(const Str& str, std::integer_sequence<T, index...> seq) {
//	constexpr T found = s2e_unfold(str, index...);
	static_assert(false, "Invalid tag");
	return static_cast<NoNum>(s2e_unfold(str, index...));
}

#endif

template <typename Str>
constexpr NoNum s2e(const Str& str) {
    return s2e(str, std::make_index_sequence<NumS.size()>{});
}

constexpr enum NoNum operator""_f (const char str[], std::size_t) {
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

template <size_t n>
void foo() {
    TraceX(n);
}

// check the literal type:
template <typename Str>
void checkLiteralType(Str&& str) {
	log_trace << "checkLit<Str>(Str&& str) template function: " << typeid(Str).name();
	for (char c : str) { cout << c; } cout << endl;
}

//template <typename Char>
//void checkLiteralType(const Char str[]) {
//	log_trace << "checkLit(const char str[]) plain function: " << str;
//}

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
    bar<s2e("one")>();
    bar<"one"_f>();
    bar<"two"_f>();
    bar<"blabla"_f>();
	foo<hashString("one")>();
	checkLiteralType("one");
	TraceX(typeid(char[4]).name());
	TraceX(typeid(const char[4]).name());
	const char one[] = "one";
	TraceX(typeid(const char*).name());
	checkLiteralType(one);
	return 0;
}

/*
template <int N>
struct string_litteral {
	constexpr string_litteral(const char (&str)[N]) {
		std::copy_n(str, N, value);
	}
	char value[N+1]{};
};

template <string_litteral x>
void foo() {
	TraceX(x.value);
}

void test() {
	foo<"qwerty">();
}
*/
