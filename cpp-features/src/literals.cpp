#include "trace.hpp"

#include <array>
#include <algorithm>
#include <iostream>
#include <string>
#include <string_view>

using namespace std;

auto x = hash<string>(string("ghjk"));
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
[[maybe_unused]] constexpr array<const char*, 4> NumS { "none", "one", "two", "three"};
//[[maybe_unused]] constexpr array<const char*, 4> NumS { "none", "one", "two", "three"};
#endif
std::ostream& operator<<(ostream& os, enum Num n) {
	auto s = NumS[static_cast<size_t>(n)];
	return os << s;
}

#if 0

constexpr bool enum2bool(Num e) {
	return e != Num::none;
}

#if (__cplusplus >= 201700)
template<typename T, T... index>
constexpr enum Num str2enum(const char* str, std::integer_sequence<T, index...> seq)
{
	Num found = Num::none;
//	( (found = std::get<index>(NumS) == string_view(str) ? static_cast<Num>(index) : found ), ...);
	( enum2bool(found = (std::get<index>(NumS) == string_view(str)) ? static_cast<Num>(index) : found ) || ...);
	return found;
}
#else // 2014
template <size_t index>
constexpr auto cmp(const char* str, Num& found){ return std::get<index>(NumS) == (str);
//constexpr auto cmp(const char* str, Num& found){ found = std::get<index>(NumS) == (str) ? static_cast<Num>(index) : found;
//return 0;
};

template<typename T, T... index>
constexpr enum Num str2enum(const char* str, std::integer_sequence<T, index...> seq)
{
	Num found = Num::none;
//	( (found = std::get<index>(NumS) == string_view(str) ? static_cast<Num>(index) : found ), ...);
//	( enum2bool(found = (std::get<index>(NumS) == string_view(str)) ? static_cast<Num>(index) : found ) || ...);
	using unused = int[];

	int val = 1;
//	(void)unused{ ((found = std::get<index>(NumS) == (str) ? static_cast<Num>(index) : found ), ++val)... };
	(void)unused{ (index + 5) ... };

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
#endif // 0

/**
 * If I don't need the enum names
 * then I can I use the literal as a typed named constant
 */

//constexpr int cmp(int index) { return index; }
template <int index>
constexpr int cmp(const char* s) { return get<index>(NumS) == s; }

enum class NoNum {}; // Empty
#if (__cplusplus >= 201700)
[[maybe_unused]] constexpr array NoNumS { "none", "one", "two", "three"};
#else
// Use make_array
[[maybe_unused]] constexpr array<const char*, 4> NoNumS { "none", "one", "two", "three"};
#endif
template<typename T, T... index>
constexpr enum NoNum s2e(const char* str, std::integer_sequence<T, index...> seq) {
//	return static_cast<NoNum>(str[0] + str[1]); ok
	return static_cast<NoNum>(hash<string>(string(str)));
	int found = -1;
    using unused = int[];

	[[maybe_unused]] int val = 1;
//    (void)unused{ ((found = std::get<index>(NumS) == string_view(str) ? index : found ), 0)... };
//    (void)unused{ ((found = std::get<index>(NumS) == str ), val)... };
//    (void)unused{ (found = index * (str == nullptr), ++val)... }; // ok
//    (void)unused{ (found = index * (get<index>(NumS) == nullptr), ++val)... }; // ok
//    (void)unused{ (found = index * (get<index>(NumS) == str), ++val)... }; // No
//    (void)unused{ (found = index * (sizeof(get<index>(NumS))), ++val)... }; // ok
    (void)unused{ (found = cmp<index>(str), ++val)... }; // ok
//	[[maybe_unused]] unused xxx{ (std::get<index>(NumS)  ? 1 : 0)... };


    return static_cast<NoNum>(found);
}


constexpr enum NoNum s2e(const char* str) {
    return s2e(str, std::make_index_sequence<NoNumS.size()>{});
}

constexpr enum NoNum operator""_f (const char* str, std::size_t) {
    return s2e(str);
}

std::ostream& operator<<(ostream& os, enum NoNum n) {
    auto i = static_cast<size_t>(n);
    return os << (i < NoNumS.size() ? NoNumS[i] : "Invalid");
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