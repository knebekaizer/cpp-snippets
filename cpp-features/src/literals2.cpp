#include "trace.hpp"
#include "cpp14_stubs.hpp"

#include <cassert>
#include <utility>
#include <array>
#include <algorithm>
#include <iostream>
#include <string>
#include <string_view>

using namespace std;

enum class EValue;

// Actual size will be declared in specialization
template <typename T, typename ...X>
constexpr static std::array<const char*, 0> Tags{};


template <>
constexpr auto Tags<EValue> = cpp14_stubs::make_array(
    "none", "one", "two" ,"three"
);
using Num = EValue;


enum class EValue { none, one, two, three};


namespace details {
template <typename E, typename T, T... index> constexpr E s2e(const char* str, std::integer_sequence<T, index...> seq) {
    constexpr T invalid = static_cast<T>(-1);
    T found = invalid;
    constexpr auto isValid = [invalid](T x) { return x != invalid; };
    (isValid(found = (std::get<index>(Tags<E>) == std::string_view(str)) ? index : found) || ...);
    //	int x[found] = {};
    //	static_assert(found != invalid, "Unknown tag");
    return static_cast<E>(found);
}


template <typename E> constexpr E s2e(const char* str) {
    return s2e<E>(str, std::make_integer_sequence<int, Tags<E>.size()>{});
}


template <typename E> auto e2s(E e) {
//    assert(("Element not present in the Tags", static_cast<size_t>(e) < Tags<E>.size()));
    return Tags<E>.at(static_cast<size_t>(e));
}
} // namespace details

template <typename T, typename std::enable_if<std::is_enum<T>::value, bool>::type = true>
std::ostream& operator<<(std::ostream& os, T e) { return os << details::e2s<T>(e); }

/// Tag suffix `_v` for Equivalence class: Value
constexpr EValue operator""_v (const char* str, std::size_t);

// postgen
constexpr EValue operator""_v(const char* str, std::size_t) {
    return details::s2e<EValue>(str);
}

/*
#if 1

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


#endif // 0

//
// If I don't need the enum names
// then I can I use the literal as a typed named constant
//

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
	return static_cast<NoNum>(hash<string>()(str));
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
*/

template <EValue n>
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

    try {
        bar<static_cast<EValue>(21)>();
    } catch (exception& e) {
        log_error << e.what();
    }
    try {
        bar<"one"_v>();
        bar<"two"_v>();
        bar<"blabla"_v>();
    } catch (exception& e) {
        log_error << e.what();
    }
	return 0;
}