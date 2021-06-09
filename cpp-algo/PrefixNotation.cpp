#include <vector>
#include <algorithm>
#include <iostream>
#include <optional>
#include <exception>

#include "trace.h"

using namespace std;
using namespace std::literals;

using T = int;
T calc(char func, T a, T b) {
	switch (func) {
		case '+':
			return a + b;
		case '*':
			return a * b;
		default:
			throw runtime_error("Bad func");
	}
}

optional<int> parseAndCalc(istream& is) {
	string token;
	if (is >> token) {
		if (token == "(") {
			char func;
			is >> func;
			auto result = parseAndCalc(is);
			while (auto op = parseAndCalc(is)) {
				result = calc(func, result.value(), op.value());
			}
			return result;
		} else if (token == ")") {
			return {};
		} else {
			// value expected
			T v = stoi(token);
			return v;
		}
	}
	return {};
}

void test(string inp) {
	istringstream is1(inp);
	auto v = parseAndCalc(is1);

	TraceX(v.value(), inp);
//	TraceX(parseAndCalc(is1).value())
}

int main() {
	test("( * ( + 1 1 ) ( +1 1 ) )"s);
	test("-5");
	test("( * -3 -2)");
	return 0;
}