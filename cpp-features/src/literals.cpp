#include "trace.hpp"

#include <array>
#include <algorithm>
#include <iostream>
#include <string>
#include <string_view>

using namespace std;

#if (__cplusplus > 201703L) // __cpp_nontype_template_args >= 201911
template <auto N>
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

#else
void test() {
	log_warn << "Not implemented";
}
#endif

enum class Num { none, one, two, three};
//[[maybe_unused]] constexpr array Nums { Num::none, Num::one, Num::two, Num::three };
#if (__cplusplus >= 201700)
[[maybe_unused]] constexpr array NumS { "none"sv, "one"sv, "two"sv, "three"sv};
//[[maybe_unused]] constexpr array NumS { "none", "one", "two", "three"};
#else
[[maybe_unused]] constexpr array<string_view, 4> NumS { "none"sv, "one"sv, "two"sv, "three"sv};
//[[maybe_unused]] constexpr array<const char*, 4> NumS { "none", "one", "two", "three"};
#endif
std::ostream& operator<<(ostream& os, enum Num n) {
	auto s = NumS[static_cast<size_t>(n)];
	return os << s;
}

constexpr bool enum2bool(Num e) {
	return e != Num::none;
}

#if (__cplusplus >= 202300)
template<typename T, T... index>
constexpr enum Num str2enum(const char* str, std::integer_sequence<T, index...> seq)
{
	Num found = Num::none;
//	( (found = std::get<index>(NumS) == string_view(str) ? static_cast<Num>(index) : found ), ...);
	( enum2bool(found = (std::get<index>(NumS) == string_view(str)) ? static_cast<Num>(index) : found ) || ...);
	return found;
}
#else // 2014
// FixMe
//template<typename T, T v>
//constexpr enum Num str2enum(const char* str, T index) {
//	Num found = (std::get<index>(NumS) == string_view(str)) ? static_cast<Num>(index) : Num::none;
//    return found;
//}
//
//template<typename T, T, T... index>
//constexpr enum Num str2enum(const char* str, T i0, std::integer_sequence<T, index...> seq)
//{
//	if (std::get<i0>(NumS) == string_view(str))
//		return static_cast<Num>(i0);
//	return str2enum(str, index...);
//}
template<typename T, T... index>
constexpr enum Num str2enum(const char* str, std::integer_sequence<T, index...> seq)
{
	Num found = Num::none;
//	( (found = std::get<index>(NumS) == string_view(str) ? static_cast<Num>(index) : found ), ...);
//	( enum2bool(found = (std::get<index>(NumS) == string_view(str)) ? static_cast<Num>(index) : found ) || ...);
	using unused = int[];

	int val = 1;
	(void)unused{ ((found = std::get<index>(NumS) == (str) ? static_cast<Num>(index) : found ), ++val)... };

	return found;
}

#endif

constexpr enum Num str2enum(const char* str) {
	return str2enum(str, std::make_index_sequence<NumS.size()>{});
}

constexpr enum Num operator""_tag (const char* str, std::size_t)
{
//	if (auto it = find(NumS.begin(), NumS.end(), str); it != NumS.end())
//		return Nums[it - NumS.begin()];

//	if ("one"sv == str) return Num::one;
//	if ("two"sv == str) return Num::two;
//	if ("three"sv == str) return Num::three;
//	return Num::none;

	return str2enum(str);
}

template <enum Num n>
void bar() {
	TraceX(n);
}

template <char ...C> std::string operator ""_s() {
	string arr{C...};
	return arr;
}

std::string operator ""_s(char c) {
	return std::string(1,c);
}

#if __cplusplus >= 202002  // __cpp_nontype_template_args

struct S {
	constexpr static char s[] = "qwertyui";
};
template <S v> void fooS() { // since C++20
	log_trace << v.s;
}
void testFooS() {
	constexpr S s;
	fooS<s>();
}

struct S2 {
	char s[32];
};
template <S2 v> void fooS2() { // since C++20
	log_trace << v.s;
}
void testFooS2() {
	fooS2<S2{"asdfghj"}>();
}
#else // __cplusplus >= 202002
//#warning <user type> nontype template args not suppported
#endif // __cplusplus >= 202002

int main() {
	test();
	[[maybe_unused]] constexpr auto t = "one"_tag;
	bar<"one"_tag>();
	bar<"two"_tag>();
	bar<"blabla"_tag>();

	TraceX('a'_s);
	log_trace << 0xa_s;
	log_trace << 0706_s;
	log_trace << 123_s;
	log_trace << 12345678_s;

	TraceX((string{65,66,67}));
	TraceX((string{'a', 'b', 'c'}));
	return 0;
}