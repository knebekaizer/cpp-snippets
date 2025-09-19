//
// Created by Vladimir on 2024-09-24.
//

#include "trace.hpp"
#include <regex>

using namespace std;

void test_search(const std::string& in, const std::string& re) {
	std::smatch m;
	std::regex_search(in, m, std::regex(re));
	if (!m.empty())
	{
		std::cout << "input=[" << in << "], regex=[" << re << "]\n  "
		                                                      "prefix=[" << m.prefix() << "]\n  smatch: ";
		for (std::size_t n = 0; n < m.size(); ++n)
			std::cout << "m[" << n << "]=[" << m[n] << "] ";
		std::cout << "\n  suffix=[" << m.suffix() << "]\n";
	}
	else
		std::cout << "input=[" << in << "], regex=[" << re << "]: NO MATCH\n";
}

void test_match(const std::string& in, const std::string& re) {
	std::smatch m;
	std::regex_match(in, m, std::regex(re));
	if (!m.empty())
	{
		std::cout << "input=[" << in << "], regex=[" << re << "]\n  "
		                                                      "prefix=[" << m.prefix() << "]\n  smatch: ";
		for (std::size_t n = 0; n < m.size(); ++n)
			std::cout << "m[" << n << "]=[" << m[n] << "] ";
		std::cout << "\n  suffix=[" << m.suffix() << "]\n";
	}
	else
		std::cout << "input=[" << in << "], regex=[" << re << "]: NO MATCH\n";
}

// #include <regex>
// #include <iostream>

int test_tl()
{
	using namespace std::regex_constants;
	std::regex e {"\\b.*"};
	const std::string s {"that that"};
	match_flag_type flags = match_not_bow;

	bool r = std::regex_search(s.begin(), s.end(), e, flags);
	std::cout << "Result: " << r << std::endl;
	return 0;
}

int main() {
	test_tl();
	return 0;
	// Alternatives
	test_search("abcdef", "abc|def");
	test_search("abc", "ab|abc"); // left Alternative matched first
	test_search("abc", "((a)|(ab))((c)|(bc))");

	// Terms
	test_search("abcdef", ""); // empty regex is a single empty Alternative
	test_search("abc", "abc|"); // left Alternative matched first
	test_search("abc", "|abc"); // left Alternative matched first, leaving abc unmatched

	// Quantifiers
	test_search("abcdefghi", "a[a-z]{2,4}"); // greedy
	test_search("abcdefghi", "a[a-z]{2,4}?"); // non-greedy
	test_search("aabaac", "(aa|aabaac|ba|b|c)*");
	test_search("aaaaaaaaaa,aaaaaaaaaaaaaaa", "^(a+)\\1*,\\1+$");
	test_search("zaacbbbcac", "(z)((a+)?(b+)?(c))*");

	// Assertions
	test_search("aaa", "a$");
	test_search("moo goo gai pan", "o\\b");
	test_search("baaabac", "(?=(a+))");
	test_search("baaabac", "(?=(a+))a*b\\1");
	test_search("abcdef", "(?=.*[[:lower:]])(?=.*[[:upper:]])(?=.*[[:punct:]]).{6,}");
	test_search("aB,def", "(?=.*[[:lower:]])(?=.*[[:upper:]])(?=.*[[:punct:]]).{6,}");

	// Atom
	// Subexpression
	// Backreference
	// Single character ???
	// regex_constants::icase, collate ...

	// Character classes: only d D s S w W
	// POSIX character classes:
	//   alnum, alpha, blank, cntrl, digit, graph, lower, print, punct, space, upper, xdigit
	//   [:alnum:]
	// ClassAtomCollatingElement(C++ only) (localization)
	//   [. ClassName .]
	// ClassAtomEquivalence
	//   [= ClassName =] // [[=a=]] matches any of the characters: a, À, Á, Â, Ã, Ä, Å, A, à, á, â, ã, ä, å

    test_match("abcdef", "abc|def");

    std::cout << '\n' << std::regex_replace("Quick brown fox", regex("a|e|i|o|u"), "[$&]") << '\n';

    std::cout << '\n' << std::regex_replace("Quick brown fox", regex("(a|e|i|o|u)"), "[\\1]"
                                    , regex_constants::format_sed) << '\n';

    std::cout << '\n' << std::regex_replace("Quick brown fox", regex("(a|e|i|o|u)"), "[\\1]"
                                    , regex_constants::format_sed | regex_constants::format_first_only) << '\n';
}
