#include "trace.hpp"

#include <array>
#include <algorithm>
#include <iostream>

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

enum class Num { zero, one, two, three};
//[[maybe_unused]] constexpr array Nums { Num::zero, Num::one, Num::two, Num::three };
[[maybe_unused]] constexpr array NumS { "zero"sv, "one"sv, "two"sv, "three"sv};

std::ostream& operator<<(ostream& os, enum Num n) {
	auto s = NumS[static_cast<size_t>(n)];
	return os << s;
}


template<typename T, T... index>
constexpr enum Num getNum(const char* str, std::integer_sequence<T, index...> seq)
{
	Num found = Num::zero;
	( (found = std::get<index>(NumS) == str ? static_cast<Num>(index) : found ), ...);
	return found;
}

constexpr enum Num getNum(const char* str) {
	return getNum(str, std::make_index_sequence<NumS.size()>{});
}


constexpr enum Num operator""_tag (const char* str, std::size_t)
{
//	if (auto it = find(NumS.begin(), NumS.end(), str); it != NumS.end())
//		return Nums[it - NumS.begin()];

//	if ("one"sv == str) return Num::one;
//	if ("two"sv == str) return Num::two;
//	if ("three"sv == str) return Num::three;
//	return Num::zero;

	return getNum(str);
}

template <enum Num n>
void bar() {
	TraceX(n);
}

template <char ...C> std::string operator ""_s() {
	string arr{C...};
	return arr;
}

int main() {
	test();
	[[maybe_unused]] constexpr auto t = "one"_tag;
	bar<"one"_tag>();
	bar<"two"_tag>();
	bar<"blabla"_tag>();

	log_trace << 123_s;
	log_trace << 12345678_s;
	return 0;
}