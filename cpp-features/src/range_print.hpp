#ifndef CPP_FEATURES_RANG_PRINT_HPP
#define CPP_FEATURES_RANG_PRINT_HPP

#include <version>
#include <ostream>

#ifndef STD_RANGES
#ifdef __cpp_lib_ranges
#define STD_RANGES 1
#else
#define STD_RANGES 0
#endif
#endif

#if STD_RANGES
#include <ranges>
namespace rg = std::ranges;
#else
#include "range/v3/all.hpp"
namespace rg = ::ranges;
#endif // __cpp_lib_ranges
namespace vs = rg::views;


#if STD_RANGES
#if 1
template<rg::input_range V>
requires (!std::convertible_to<V, std::string>)
std::ostream& operator<<(std::ostream& os, V&& view) {
	os << '[';
	auto b = view.begin();
	if (b != view.end()) {
		os << *b;
		while (++b != view.end()) os << ',' << *b;
	}
	os << ']';
	return os;
};
#else
/// Alternative variant: use const reference for containers and copy for viewa
template<rg::input_range V>
requires (!rg::view<V>)
 && (!std::convertible_to<V, std::string>)
std::ostream& operator<<(std::ostream& os, V const& view) {
	os << '[';
	auto b = view.begin();
	if (b != view.end()) {
		os << *b;
		while (++b != view.end()) os << ',' << *b;
	}
	os << ']';
	return os;
};

template<rg::view V>
std::ostream& operator<<(std::ostream& os, V view) {
	os << '[';
	auto b = view.begin();
	if (b != view.end()) {
		os << *b;
		while (++b != view.end()) os << ',' << *b;
	}
	os << ']';
	return os;
};
#endif

#else
// *** range-v3 *******************
template<rg::range V>
requires (!std::convertible_to<V, std::string>)
//	(!rg::viewable_range<V>)
//	&&
std::ostream& operator<<(std::ostream& os, V&& view) {
	os << '[';
	auto b = view.begin();
	if (b != view.end()) {
		os << *b;
		while (++b != view.end()) os << ',' << *b;
	}
	os << ']';
	return os;
};

#endif

#endif //CPP_FEATURES_RANG_PRINT_HPP
