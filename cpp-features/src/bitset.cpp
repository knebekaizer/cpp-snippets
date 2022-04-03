#include <bitset>
#include <iostream>

#include "trace.hpp"

using namespace std;


namespace _1 {
using flag_t = std::bitset<8>;
[[maybe_unused]]
constexpr flag_t fLocation = 1ul << 0;
[[maybe_unused]]
constexpr flag_t fNamespace = 1ul << 1;
[[maybe_unused]]
constexpr flag_t fWhatever = 1ul << 2;

auto mask = [](std::initializer_list<flag_t> list) {
	flag_t all = 0;
	for (auto f: list) {
		all |= f;
	}
	return all;
};

auto testAny = [](flag_t x, std::initializer_list<flag_t> all) {
	return (x & mask(all)).any();
};

auto testAll = [](flag_t x, std::initializer_list<flag_t> all) {
	auto m = mask(all);
	return (x & m) == m;
};

auto setAll = [](flag_t&& x, std::initializer_list<flag_t> all) {
	auto m = mask(all);
	return x |= m;
};

void test() {

	TraceX(flag_t(3).all());
	TraceX(flag_t(3).any());

	TraceX(testAny(flag_t(0), {fLocation, fNamespace}));
	TraceX(testAny(flag_t(1), {fLocation, fNamespace}));
	TraceX(testAny(flag_t(2), {fLocation, fNamespace}));
	TraceX(testAny(flag_t(3), {fLocation, fNamespace}));
	TraceX(testAny(flag_t(4), {fLocation, fNamespace}));
	cout << endl;
	TraceX(testAll(flag_t(0), {fLocation, fNamespace}));
	TraceX(testAll(flag_t(1), {fLocation, fNamespace}));
	TraceX(testAll(flag_t(2), {fLocation, fNamespace}));
	TraceX(testAll(flag_t(3), {fLocation, fNamespace}));
	TraceX(testAll(flag_t(4), {fLocation, fNamespace}));
	cout << endl;
	TraceX(testAny(setAll(0, {fWhatever}), {fLocation, fNamespace}));
	TraceX(testAny(setAll(0, {fWhatever, fLocation}), {fLocation, fNamespace}));
	TraceX(testAny(setAll(0, {fWhatever, fNamespace}), {fLocation, fNamespace}));
	TraceX(testAny(setAll(0, {fWhatever, fLocation, fNamespace}), {fLocation, fNamespace}));
	TraceX(testAny(setAll(0, {fNamespace}), {fLocation, fNamespace}));
	cout << endl;
	TraceX(testAll(setAll(0, {fWhatever}), {fLocation, fNamespace}));
	TraceX(testAll(setAll(0, {fWhatever, fLocation}), {fLocation, fNamespace}));
	TraceX(testAll(setAll(0, {fWhatever, fNamespace}), {fLocation, fNamespace}));
	TraceX(testAny(setAll(0, {fWhatever, fLocation, fNamespace}), {fLocation, fNamespace}));
	TraceX(testAll(setAll(0, {fNamespace}), {fLocation, fNamespace}));
}

} // _1

namespace _2 {

using flag_t = uint8_t;
[[maybe_unused]]
constexpr flag_t fLocation = 1ul << 0;
[[maybe_unused]]
constexpr flag_t fNamespace = 1ul << 1;
[[maybe_unused]]
constexpr flag_t fWhatever = 1ul << 2;

auto mask = [](std::initializer_list<flag_t> list) {
	flag_t all = 0;
	for (auto f: list) {
		all |= f;
	}
	return all;
};

auto testAny = [](flag_t x, std::initializer_list<flag_t> all) -> bool {
	return x & mask(all);
};

auto testAll = [](flag_t x, std::initializer_list<flag_t> all) -> bool {
	auto m = mask(all);
	return (x & m) == m;
};

auto setAll = [](flag_t&& x, std::initializer_list<flag_t> all) -> flag_t {
	auto m = mask(all);
	return x |= m;
};

void test() {
	TraceX(testAny(flag_t(0), {fLocation, fNamespace}));
	TraceX(testAny(flag_t(1), {fLocation, fNamespace}));
	TraceX(testAny(flag_t(2), {fLocation, fNamespace}));
	TraceX(testAny(flag_t(3), {fLocation, fNamespace}));
	TraceX(testAny(flag_t(4), {fLocation, fNamespace}));
	cout << endl;
	TraceX(testAll(flag_t(0), {fLocation, fNamespace}));
	TraceX(testAll(flag_t(1), {fLocation, fNamespace}));
	TraceX(testAll(flag_t(2), {fLocation, fNamespace}));
	TraceX(testAll(flag_t(3), {fLocation, fNamespace}));
	TraceX(testAll(flag_t(4), {fLocation, fNamespace}));
	cout << endl;
	TraceX(testAny(setAll(0, {fWhatever}), {fLocation, fNamespace}));
	TraceX(testAny(setAll(0, {fWhatever, fLocation}), {fLocation, fNamespace}));
	TraceX(testAny(setAll(0, {fWhatever, fNamespace}), {fLocation, fNamespace}));
	TraceX(testAny(setAll(0, {fWhatever, fLocation, fNamespace}), {fLocation, fNamespace}));
	TraceX(testAny(setAll(0, {fNamespace}), {fLocation, fNamespace}));
	cout << endl;
	TraceX(testAll(setAll(0, {fWhatever}), {fLocation, fNamespace}));
	TraceX(testAll(setAll(0, {fWhatever, fLocation}), {fLocation, fNamespace}));
	TraceX(testAll(setAll(0, {fWhatever, fNamespace}), {fLocation, fNamespace}));
	TraceX(testAll(setAll(0, {fWhatever, fLocation, fNamespace}), {fLocation, fNamespace}));
	TraceX(testAll(setAll(0, {fNamespace}), {fLocation, fNamespace}));
}

}

/*
namespace _3 {

using flag_base = uint8_t;

enum class Flags : uint8_t {
	fLocation = 1ul << 0,
	fNamespace = 1ul << 1,
	fWhatever = 1ul << 2
};

using flag_t = Flags;

auto mask = [](std::initializer_list<flag_t> list) {
	flag_base all = 0;
	for (auto f: list) {
		all |= static_cast<flag_base>(f);
	}
	return all;
};

auto testAny = [](flag_t x, std::initializer_list<flag_t> all) -> bool {
	return static_cast<flag_base>(x) & mask(all);
};

auto testAll = [](flag_t x, std::initializer_list<flag_t> all) -> bool {
	auto m = mask(all);
	return (static_cast<flag_base>(x) & m) == m;
};

auto setAll = [](flag_t&& x, std::initializer_list<flag_t> all) -> flag_t {
	auto m = mask(all);
	x = static_cast<flag_t>(static_cast<flag_base>(x) |= m)
	return x;
};

void test() {
	TraceX(testAny(flag_t(0), {fLocation, fNamespace}));
	TraceX(testAny(flag_t(1), {fLocation, fNamespace}));
	TraceX(testAny(flag_t(2), {fLocation, fNamespace}));
	TraceX(testAny(flag_t(3), {fLocation, fNamespace}));
	TraceX(testAny(flag_t(4), {fLocation, fNamespace}));
	cout << endl;
	TraceX(testAll(flag_t(0), {fLocation, fNamespace}));
	TraceX(testAll(flag_t(1), {fLocation, fNamespace}));
	TraceX(testAll(flag_t(2), {fLocation, fNamespace}));
	TraceX(testAll(flag_t(3), {fLocation, fNamespace}));
	TraceX(testAll(flag_t(4), {fLocation, fNamespace}));
	cout << endl;
	TraceX(testAny(setAll(0, {fWhatever}), {fLocation, fNamespace}));
	TraceX(testAny(setAll(0, {fWhatever, fLocation}), {fLocation, fNamespace}));
	TraceX(testAny(setAll(0, {fWhatever, fNamespace}), {fLocation, fNamespace}));
	TraceX(testAny(setAll(0, {fWhatever, fLocation, fNamespace}), {fLocation, fNamespace}));
	TraceX(testAny(setAll(0, {fNamespace}), {fLocation, fNamespace}));
	cout << endl;
	TraceX(testAll(setAll(0, {fWhatever}), {fLocation, fNamespace}));
	TraceX(testAll(setAll(0, {fWhatever, fLocation}), {fLocation, fNamespace}));
	TraceX(testAll(setAll(0, {fWhatever, fNamespace}), {fLocation, fNamespace}));
	TraceX(testAll(setAll(0, {fWhatever, fLocation, fNamespace}), {fLocation, fNamespace}));
	TraceX(testAll(setAll(0, {fNamespace}), {fLocation, fNamespace}));
}

}
*/

int main() {
	_1::test();
	cout << "\n --------------------------------- \n";
	_2::test();
//	assert(msb(1 << 5) == 5);
return 0;
}