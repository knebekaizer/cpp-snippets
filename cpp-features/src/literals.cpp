#include "trace.hpp"

#include <algorithm>

#if (__cplusplus > 201703L) // __cpp_nontype_template_args >= 201911
template <auto N>
struct string_litteral {
	constexpr string_litteral(const char (&str)[N]) {
		std::copy_n(str, N, value);
	}

	char value[N];
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

int main() {
	test();
	return 0;
}