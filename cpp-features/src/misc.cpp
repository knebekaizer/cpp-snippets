#include <iostream>
#include <array>
#include <cassert>
#include <memory>
#include <type_traits>

#define USE_RUNTIME_LOG_LEVEL
#include "trace.hpp"
LOG_LEVEL::LOG_LEVEL gLogLevel = LOG_LEVEL::trace;

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

class B {};
class D : public B {};
unique_ptr<B> test_uptr() {
    return unique_ptr<B>(new D());
}

/// Factory: Can I use const_expr to build the function map (factory)?
//#include <array>
//class Base {};
//using FuncT = Base* (*)();
//constexpr std::array<FuncT*, 16> factoryMap{};
//Base* foo();
//constexpr factoryMap[8] = foo; // Nope

class Smth {
public:
    struct Dummy {
        Dummy() = delete;
        void foo() const { TraceF; };
    };
    static Dummy dummy;
    void bar() const { TraceF; }
};
//Smth::Dummy Smth::dummy;

void testStaticMemberInit() {
    Smth().bar();

}

void testHierarchy() {
    struct A { virtual ~A() = default; }; A* a = new A;
    struct B : A {}; A* b = new B; B* bb = new B; B* z = nullptr;
    struct C : B {}; A* c = new C;

    cout << boolalpha;
    TraceX(typeid(z) == typeid(B*));     // true
    TraceX(typeid(a) == typeid(b));     // true
    TraceX(typeid(bb) == typeid(b));    // false
    TraceX((bool)dynamic_cast<B*>(z), typeid(z) == typeid(B*));
    TraceX((bool)dynamic_cast<B*>(a), typeid(a) == typeid(B*));
    TraceX((bool)dynamic_cast<B*>(b), typeid(b) == typeid(B*));
    TraceX((bool)dynamic_cast<B*>(c), typeid(b) == typeid(B*));
    TraceX((bool)dynamic_cast<B*>(bb), typeid(bb) == typeid(B*));

}

namespace {
int loggerCount = 0;
}
#include "scoped.hpp"
void testLogger() {
    struct Str {
        Str() { ++loggerCount; }
        int count() const { return loggerCount; }
    };
    log_trace << Str().count();
    gLogLevel = LOG_LEVEL::warn;
    log_trace << Str().count();
    log_trace << Str().count();
    gLogLevel = LOG_LEVEL::trace;
    log_trace << Str().count();
}

/*
template <class...> constexpr std::false_type always_false{};
if constexpr(condition1) {
	...
} else if constexpr (condition2) {
	...
} else if constexpr (condition3) {
	...
} else {
	static_assert(always_false<T>);
}
*/

namespace boolWrapper {
string g_s = "abc";
int ln = 0;
bool isSameBuf(string const& s) { return (s.data() == g_s.data()); }
void foo(auto x, string const& s) { log_trace << x << ", " << (s==g_s) << ", " << isSameBuf(s); }
void bar(auto x, string&& s)  { log_trace << x << ", " << (s==g_s) << ", " << isSameBuf(s); }
}
//static void foo(string s) { TraceX(s); }
void ex_boolWrapper() {
	using namespace boolWrapper;
	struct XString {
		XString(std::string& s) : s_(s) {}
		operator std::string const&() const& { return s_; }   // NOLINT(google-explicit-constructor)
		operator std::string&() & { return s_; }              // NOLINT(google-explicit-constructor)
		operator std::string&&() && { return std::move(s_); } // NOLINT(google-explicit-constructor)
		explicit operator bool() const { return !s_.empty(); }
		std::string& s_;
	};

	auto check = [](auto const& s){ return s ? "not empty" : "empty"; };
	XString xs = g_s;
	foo(1, xs.s_);

	string ss = xs; // copy, as expected
	foo(2, xs);

	auto xss = xs;
	foo(3, xss);    // no copy; expected copy

	foo(4, ss);
//	bar(xs);    // won't compile - as expected
	TraceX(check(xs), g_s);
	bar(5, std::move(xs)); // NOLINT(performance-move-const-arg)
	TraceX(check(xs), g_s);
//	TraceX(check(ss), g_s);  // OK: no viable conversion from 'const std::string' to 'bool'
	g_s.resize(0);
	TraceX(check(xs), g_s);

	struct String : public std::string {
		String() = delete;
		explicit operator bool() const { return !empty(); }
	};
	g_s = "cba";
	String& a = static_cast<String&>(g_s);
	foo(11, a);
	auto b = a;
	foo(12, b);

	TraceX(check(a), g_s);
	bar(13, std::move(a)); // no move, reference copied
	TraceX(check(a), g_s);
	bar(14, std::move(std::remove_reference<decltype(a)>::type(a))); // bar> 14, true, false  WTF? it's copy
	TraceX(check(a), g_s);
}

#include <charconv>
#include <cstdint>
using int128_t = __int128;
using uint128_t = unsigned __int128;

uint128_t x2u128(string_view s) {
	auto s1 = "5e1463677a8246bf2f99ea81f6baf1a6"sv;
	auto s2 = "5e1463677a8246bf2f99ea81f6baf1a7"sv;
	uint128_t x1, x2;
	from_chars(s1.begin(), s1.end(), x1, 16);
	from_chars(s2.begin(), s2.end(), x2, 16);
	TraceX(int(x2 - x1));
	return x1;
}

template <class _Key, class _Value> struct __value_type;
template <class _Tp>
struct __tree_key_value_types {};

template <class _Key, class _Tp>
struct __tree_key_value_types<__value_type<_Key, _Tp> > {};


struct S {
	S() { ++count; }
	~S() { --count; }
	static int count;
};
int S::count = 0;
void shared() {
	{
		auto x = make_shared<S>();
		TraceX(x->count);
		assert(x.use_count() == 1);
		auto x1 = x;
		assert(x.use_count() == 2);
		assert(S::count == 1);
	}
	TraceX(S::count);
	assert(S::count == 0);
}

struct S2 {
	S2() : flag(1) {}
	S2(int x) : flag(2) {}
	S2(const char* s) : flag(4) {}
	S2(int a, double b) : flag(8) {}
	unsigned flag = 0;
};
void shared2() {
	auto s0 = make_shared<S2>();
	assert(s0->flag == 1);
	auto s1 = make_shared<S2>(42);
	assert(s1->flag == 2);
	auto s3 = make_shared<S2>("qwerty");
	assert(s3->flag == 4);
	auto s4 = make_shared<S2>(0, 0.0);
	assert(s4->flag == 8);
}

template <typename T, typename S>
bool string_ctor(T begin, T end, S not_used) {
	S s(begin, end);
	S q(static_cast<typename S::size_type>(begin), static_cast<typename S::value_type>(end));
	return s == q;
}

void string_test() {
	assert((string_ctor(4, 16, string()) == true));
	assert((string_ctor(4ull, 16ull, u16string()) == true));
	vector<int> v = {65, 66, 67, 68};
	string s(v.begin(), v.end());
	assert(s == "ABCD");
	TraceX(s);
	log_trace << "PASSED";
}

int main() {
	shared();
	shared2();
	string_test();
	return 0;

	log_info << "Start";
	TraceX(getOsName());

	myTemplateFunc(42);
	myTemplateFunc("Hello, world!");
	std::cout << "Hello, world!" << std::endl;

	TraceX(msb(1 << 5));
	assert(msb(1 << 5) == 5);
	test_scanf();
	test_endian();

	testStaticMemberInit();
	testHierarchy();

	testLogger();
	ex_boolWrapper();
	x2u128("5e1463677a8246bf2f99ea81f6baf1a6");

	return 0;
}
