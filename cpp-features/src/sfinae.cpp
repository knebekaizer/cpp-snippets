//
// Created by Vladimir on 2023-10-31.
//

#include "trace.hpp"

#include <utility>
#include <typeinfo>

using namespace std;

template <typename T1, typename T2>
typename std::enable_if<std::is_same<T1,T2>::value, bool>::type
test(T1 a = {}, T2 b = {}) {
	log_trace << "Not implemented, or template parameter types are not supported: "
				 "T1: " << typeid(T1).name() << "; T2: " << typeid(T2).name();
	return false;
}

template <typename T1, typename T2>
typename std::enable_if<!std::is_same<T1,T2>::value, bool>::type
test(T1 a = {}, T2 b = {}) {
	log_trace << "Types are: " << typeid(T1).name() << " and " << typeid(T2).name();

	pair<T1, T2> p{a, b};
	[[maybe_unused]] auto first = get<T1, T2>(p);
	[[maybe_unused]] auto second = get<T2, T1>(p);
	TraceX(first, second);
	return true;
}

template <typename T1, typename T2, typename std::enable_if<std::is_same<T1,T2>::value, bool>::type = true>
pair<T2, T1> revert_pair(const pair<T1, T2>&) {
	log_trace << "Not implemented, or template parameter types are not supported: "
	             "T1: " << typeid(T1).name() << "; T2: " << typeid(T2).name();
	return {};
}

template <typename T1, typename T2, typename std::enable_if<!std::is_same<T1,T2>::value, bool>::type = true>
pair<T2, T1> revert_pair(const pair<T1, T2>& p) {
	cout << "Types are: " << typeid(T1).name() << " and " << typeid(T2).name() << endl;
	auto first = get<T1, T2>(p);
	auto second = get<T2, T1>(p);
	return {second, first};
}

template <typename T1, typename T2>
void test_revert(const T1& first, const T2& second) {
	auto a = pair(first, second);
	auto b = revert_pair(a);
	if (a == revert_pair(b))
		cout << "PASSED";
	else
		cout << "FAILED";
	cout << endl;
}


int main() {
	test<int,long>();
	test<int,int>();

	char s1[] = "foo";
	test<const char*, char*>("bar", s1);

	const signed char s2[] = "foo";
	test<const char*, const signed char*>("bar", s2);

	test_revert(1, 2l);
	test_revert(1, 2);

	return 0;
}