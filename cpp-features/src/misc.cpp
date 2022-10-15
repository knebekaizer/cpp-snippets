#include <iostream>
#include <array>
#include <cassert>
#include <type_traits>

#include "trace.hpp"

using namespace std;
int msb(int a) {
	if (a == 0) return -1;
	auto k = 0;
	while(a > 0) {
		a >>= 1;
		++k;
	}
	return k - 1;
}

void myTemplateFunc(auto param) {
	cout << "param: " << param << endl;
}

auto test_scanf() {
//	auto buf = "@5      call_expr        type: @2       fn  : @7"s;
	auto buf = "        type: @2       fn  : @7"s;
	char type[5] = {0};
	char fn[3] = {0};
	int type_, fn_;
	TraceX(buf);
	auto rc = sscanf(buf.c_str(), "%4s %*[ :] %*[ @] %d %2s %*[ :] %*[ @] %d", type, &type_, fn, &fn_);
	TraceX(rc, type, fn);
	TraceX(type_, fn_);

	rc = sscanf(buf.c_str(), "type %*[ :] %*[ @] %d fn %*[ :] %*[ @] %d", &type_, &fn_);
	TraceX(rc, type, fn);
	TraceX(type_, fn_);
}

void test_ctor() {
    [[maybe_unused]] // test compilation only
    std::array<int, 5> arr1{{3, 4, 5, 1, 2}}; // Why I need double {{ ?
    [[maybe_unused]] // test compilation only
    std::array<int, 5> arr2{3, 4, 5, 1, 2};
}

#include <sys/utsname.h>
//std::string EnvironmentImpl::osNameImpl()
std::string getOsName() {
    struct utsname uts;
    uname(&uts);
    return uts.sysname;
}

#include <bit>

//if constexpr (std::endian::native == std::endian::big)
//    log_trace << "big-endian\n";
//else if constexpr (std::endian::native == std::endian::little)
//    log_trace << "little-endian\n";
//else std::cout << "mixed-endian\n";

template <bool BigEndian> struct DataN;
template <>
struct DataN<false>{
    uint8_t half2:4;
    uint8_t half1:4;
};

template <>
struct DataN<true> {
    uint8_t half1:4;
    uint8_t half2:4;
};

constexpr bool BigEndian = (std::endian::native == std::endian::big);
using DataH = DataN<BigEndian>;

template <std::endian> struct XDataN;
template <> struct XDataN<std::endian::little>{
    uint8_t half2:4;
    uint8_t half1:4;
};
template <> struct XDataN<std::endian::big>{
    uint8_t half2:4;
    uint8_t half1:4;
};
using XDataH = XDataN<std::endian::native>;

void test_endian() {
    if constexpr (std::endian::native == std::endian::big)
        log_trace << "big-endian\n";
    else if constexpr (std::endian::native == std::endian::little)
        log_trace << "little-endian\n";
    else std::cout << "mixed-endian\n";

    if(*(char *)(int[]){1}) {
        log_trace << "little endian";
    } else {
        log_trace << "big endian code";
    }

    uint8_t x = 1;
    auto y = *(DataH*)&x;
    TraceX((int)y.half1, (int)y.half2);
    auto z = *(XDataH*)&x;
    TraceX((int)z.half1, (int)z.half2);
}

int main() {
    log_info << "Start";
    TraceX(getOsName());

	myTemplateFunc(42);
	myTemplateFunc("Hello, world!");
	std::cout << "Hello, world!" << std::endl;

	TraceX(msb(1 << 5));
	assert(msb(1 << 5) == 5);
	test_scanf();
    test_endian();
}
