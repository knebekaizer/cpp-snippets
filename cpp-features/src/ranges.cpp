//
// Created by Vladimir Ivanov on 01.05.2021.
//

#include <sstream>
#include <algorithm>
#include <iostream>
#include <span>

#include "range/v3/all.hpp"

#include "scoped.hpp"

#define TRACE_PRETTY
#include "trace.hpp"

using namespace ranges;
namespace rg = ranges;
namespace vs = ranges::views;

using namespace std;
using namespace std::literals;


auto streamView(std::istream&& is) {
    return rg::istream<string>(is);
}

template <typename T>
ostream& operator<<(ostream& os, const vector<T>& v) {
	if (!v.empty()) {
		os << "{" << v[0];
		std::for_each(1 + v.begin(), v.end(), [&os](const auto& x){ os << ", " << x; });
		os << "}";
	}
	return os;
}

//template <rng::input_range T>
//ostream& operator<<(ostream& os, const T& v) {
//	  if (!v.empty()) {
//		  os << "{" << v[0];
//		  std::for_each(1 + v.begin(), v.end(), [&os](const auto& x){ os << ", " << x; });
//		  os << "}";
//	  }
//	  return os;
//}

template <size_t N>
ostream& operator<<(ostream& os, const array<char, N>& v) {
	return os << &v[0];
}

template <typename T, size_t N>
ostream& operator<<(ostream& os, const array<T, N>& v) {
	if (!v.empty()) {
		os << "{" << v[0];
		std::for_each(1 + v.begin(), v.end(), [&os](const auto& x){ os << ", " << x; });
		os << "}";
	}
	return os;
}


void ex_slice() {
	auto letters = vs::iota('a','g');
	TraceX(letters);	// prints: {a,b,c,d,e,f,g}

	TraceX(letters | vs::slice(2,5));	// prints: {c,d,e}
	TraceX((letters[{3,6}]));

	auto res = letters[{2, 5}];
	vector<char> v(res.begin(), res.end());
	TraceX(v);
	cout << endl;
}


void cipherCaesar() {
	auto shift = 11;
	auto const src = "apple"s;
	TraceX(src);

	auto alphabet = views::closed_iota('a','z') | views::cycle; // [a,b,c...a,b,c...]
	auto shifted_alphabet = alphabet | views::drop(shift); // [l,m,n...a,b,c...]

	auto encrypt = views::for_each([shifted_alphabet](char letter){
		return shifted_alphabet | views::drop(letter - 'a') | views::take(1);
	}); // [l,a,a,w,p]

	TraceX(src | encrypt | to<string>);
}

//! This should be named `views::flat_map`
void test_foreach() {
	vector<int> vi = {1,2,3,4,5,6,7,8};
	auto const s = "abcdefgh"s ;

	auto s2i  = s | vs::for_each([](char k) { return vs::single(k+1); }); // list of ints
	auto s2c  = s | vs::for_each([](char k) { return vs::single((char)(k+1)); }); // list of chars
	TraceX(s, s2i, s2c);               // s2i = [98,99,100,101,102,103,104,105]; s2c = [b,c,d,e,f,g,h,i]
	TraceX(s, s2i | to<string>);    // s = abcdefgh; s2i | to<string> = bcdefghi
	auto v2 = vi | vs::for_each([](auto k) { return vs::single(k+1); });    // vi = {1, 2, 3, 4, 5, 6, 7, 8}; v2 = [2,3,4,5,6,7,8,9]
	TraceX(vi, v2);
	// Equivalent with transform (simpler)
	auto t2 = vi | vs::transform([](auto k) { return k+1; });    // vi = {1, 2, 3, 4, 5, 6, 7, 8}; v2 = [2,3,4,5,6,7,8,9]
	TraceX(vi, t2);
}

void test_toVector() {
	vector<int> v {1, 2, -3, 4, -5, 6, -7, 8, 9};
//	std::vector<int> v2 = v
//			| vs::transform([](int x){ return x * 2; })
//			| vs::filter([](int x) { return x > 3; });
	auto myConv = vs::transform([](int x){ return x * 2; })
				| vs::filter([](int x) { return x > 3; });
	auto v2 = v | myConv | to<decltype(v)>;
	TraceX(v2);

	vector<int> v3;
	rg::copy(v | myConv, rg::back_inserter(v3));
	TraceX(v3);
}

void test_shuffle() {
	vector<int> src = vs::ints(0,10) | to<decltype(src)>;  // 0, 1, 2, 3, 4, 5, 6, 7, 8, 9
	TraceX(src);
	TraceX(vs::ints(0,8));

	std::default_random_engine gen;

//	vs::ints(0,8) | actions::shuffle(gen); // invalid
//	vs::ints(0,8) | vs::cache1 | actions::shuffle(gen); // invalid
	auto sh1 = vs::ints(0,8) | to_vector | actions::shuffle(mt19937{21});
	TraceX(sh1);  // {4, 2, 3, 1, 6, 5, 0, 7} = vector

	auto t = actions::push_back(views::iota(0, 10)) | actions::shuffle(gen);
	auto v1 = std::vector<int>{} | t;
	TraceX(v1);
	TraceX(std::move(v1) | actions::shuffle(gen));
	vector<int>{1,2,3} | actions::shuffle(gen);
}

void test_transform() {
	auto s1 = "string"s;
	auto s1_res = s1 | vs::transform([](auto c) -> char { return c + 1;});
	TraceX(s1, s1_res);

	auto sv = "string_view"sv;
	auto sv_res = sv | vs::transform([](auto c) -> char { return c + 1;});
	TraceX(sv, sv_res);

	// temporary doesn't fit
//	"tmp"s | vs::transform([](auto c) -> char { return c + 1;});
}

//! Generate the array of format strings to be used in sscanf;   {%1llx, %2llx ... %16llx}
vector<string> formatStr(int n) {
//	  std::vector<char[8]> out(n+1);

	using C = array<char, 8>; //char C[8];
	array<C, 17> buf = {{{'\0'}}};
	auto fmt = views::iota(1, n+1)
			| views::transform([](auto x) { return string("%") + to_string(x) + "llx"; });
//			  | views::for_each([](auto x) { return string("%") + to_string(x) + "llx"; });

//	  std::for_each(buf.begin() + 1, buf.end(), [k=0](auto& p) mutable { views::copy( ("%"s + to_string(k) + "llx"s),  )})
	string s = fmt[0];
	std::copy(s.begin(), s.end(), buf[1].begin());
	std::copy_n(fmt[0].begin(), fmt[0].size(), buf[1].begin());
//	  auto p = buf[1]; for (auto c : fmt[0]) *p++ = c;
	TraceX(&buf[1][0]);

//	  int k = 0;
//	  for (auto const& s: fmt) std::copy(s.begin(), s.end(), &buf[++k][0]);

//	  std::for_each(fmt.begin(), fmt.end(), [p=buf.begin()](const auto& s) mutable { std::copy_n(s.begin(), s.size(), (++p)->begin()); });
//	  for (auto const& x : buf) TraceX(&x[0]);
//	  TraceX(buf);

	return {fmt.begin(), fmt.end()};
}

//! Generate the array of format strings to be used in sscanf;   {%1llx, %2llx ... %16llx}
auto test_gen() {
	constexpr int n = 16;
	auto gen = views::iota(1, n+1)
				| views::transform([](auto x) { return "%"s + to_string(x) + "llx"; });
	TraceX(gen[n]);

	static char buf[17][8] = {{0}};
	rg::for_each(gen, [p = buf](const auto& x) mutable { rg::copy(x, (*++p)); } );
//	  rng::for_each(gen, [](const auto& x)	{ TraceX(x); });
	TraceX(buf[16]);

	array<char, 8> a = {"qwerty"};
	TraceX(a);

	static std::span ret(buf);
	return ret;
}

//! Generate the array of format strings to be used in sscanf;   {%1llx, %2llx ... %16llx}
auto test_gen2() {
	auto gen_fn = []() {
		constexpr int n = 16;
		auto gen = views::iota(1, n + 1)
				| views::transform([](auto x) { return "%"s + to_string(x) + "llx"; });
		TraceX("lambda> ", gen[n]);

		static char buf[17][8] = {{0}};
//		  rng::for_each(gen, [p = buf](const auto& x) mutable { rng::copy(x, (*++p)); });


		rg::for_each(
				buf | views::drop(1),
				[k=0](auto& p) mutable { rg::copy(("%"s + to_string(++k) + "llx"), p); });

		for (int k = 0; auto& p : buf | views::drop(1))
			rg::copy(("%"s + to_string(++k) + "llx"), p);

		array<char, 8> a = {"qwerty"};
		TraceX(a);
		return &buf;
	};

	static std::span ret(*gen_fn());
	return ret;
}

void test_temporary() {
    constexpr static auto input = "Niebloid comes from Eric Niebler's name. In simple words, they are function objects"
                                  " that disable ADL (argument-dependent lookup)";
    auto wordsView = streamView(istringstream(input));
    auto sv = wordsView | to_vector; // materialized
    TraceX(sv.size());
    TraceX((wordsView | to_vector).size());  // 0; because it's temporary
    TraceX(vector(wordsView | to_vector).size());  // 0; because it's temporary ?

	vector<string> one{"one"s, "two"s};
	auto two = "two";
	TraceX(vs::c_str(two) | vs::all);
	auto twos = "twos"s;
	TraceX(twos | vs::all);
	TraceX(vs::c_str(("twos"s).c_str()) | vs::all);

	one | vs::transform([](auto s){ return s; });
	// let `transform` return a temporary:
	TraceX(one | vs::transform([](auto s){ return s; }));
	// `auto s` is a temporary; then vs::all produces dangling. Use `by ref` parameter passing, then it references stable source (container)
	cout << ">> " << (one | vs::transform([](auto& s){ return s | vs::all; })) << endl;
//	cout << ">> " << (one | vs::for_each( [](string s){ return s | vs::all; } )) << endl;
	cout << ">>> " << (one | vs::transform( [](string& s){ return s | vs::all; } ) | vs::join('\n') | to<string> ) << endl;

}

void ex_generate(int n) {
    auto fast_Fibonacci = vs::generate([a=0, b=1]() mutable{
        auto t = a;
        a = b;
        b += t;
        return b;
    });
    TraceX(fast_Fibonacci | vs::take(n));
}

void ex_foreach()
{
	auto foreach_pipe = vs::ints(1, 6) | views::for_each([](int i) { return yield_from(views::repeat_n(i, i)); });
	auto foreach_expr = views::for_each(views::ints(1, 6),
							  [](int i) { return yield_from(views::repeat_n(i, i)); }) |
			  to<std::vector>();
	// prints: [1,2,2,3,3,3,4,4,4,4,5,5,5,5,5]
	TraceX(foreach_pipe);
	TraceX(foreach_expr);
	TraceX(views::all(foreach_expr));
}


void ex_wordTokenizer() {
    constexpr static auto input = "Niebloid   comes from Eric Niebler's name.\nIn simple words, they are function objects"
                                  " that disable ADL (argument-dependent lookup)";

    auto words = vs::c_str(input) | views::split(' ');
    TraceX(words);
    TraceX(words | to<vector<string>>);
    TraceX((words | to<vector<string>>).size()); // 18

    // range of range<char>
    auto splitBetter = vs::split_when([](auto it, auto end){
        if (!isspace(*it)) return make_pair(false, it);
        while (it != end && isspace(*it)) ++it;
        return make_pair(true, it);
    } );
    auto wordsBetter = vs::c_str(input) | splitBetter;
    TraceX(wordsBetter);

    auto splitFromStream = [](auto input) {
        istringstream local(input);
        // Wrong idea: `local` owns text, so returning a view lost ownership and hangs
//        return rg::istream<string>(local); // returns temporary (from local)
        // Materialize to vector is OK but expensive
        return rg::istream<string>(local) | to_vector;
    };
    TraceX(splitFromStream(input));

    string str = input;
    auto wordsAsSV = splitBetter // range of range<char>
        | vs::transform([](auto &&rng) {
                    return std::string_view(&*rng.begin(), rg::distance(rng));
                });
    TraceX(str | wordsAsSV);
    TraceX(vs::c_str(input) | wordsAsSV);
    TraceX(string_view(input) | wordsAsSV);

    cout << endl;
}


void ex_sort() {
    constexpr static auto input = "Niebloid comes from Eric Niebler's name. In simple words, they are function objects"
                                  " that disable ADL (argument-dependent lookup)";

    auto wordsView = streamView(istringstream(input));
    auto sv = wordsView | to_vector; // materialized
    TraceX(sv.size());
    TraceX((wordsView | to_vector).size());  // 0; because it's temporary
    TraceX(vector(wordsView | to_vector).size());  // 0; because it's temporary ?
    TraceX((wordsView | to_vector | actions::sort).size()); // sort temporary: compiled, but empty
    TraceX(wordsView | to_vector | actions::sort); // sort temporary: compiled, but empty
    TraceX(0, rg::is_sorted(sv)); // sv now is sorted
    auto copy = vs::all(sv) | actions::sort;
    TraceX(boolalpha, (&copy[0] == &sv[0]));  // not a view; full copy
    TraceX(sv | vs::all | actions::sort); // this is NOT a temporary
    TraceX(1, rg::is_sorted(sv)); // sv now is sorted too
    sv = streamView(istringstream(input)) | to_vector; // run again
    TraceX(2, vs::all(sv) | actions::sort); // the same as above
    TraceX(2, rg::is_sorted(sv)); // sv now is sorted


    rg::sort(sv, {}, &string::size);
    auto sortedBack = vs::all(sv) | actions::sort([](const string& a, const string& b) { return a.back() < b.back();});
    TraceX(sortedBack);

//    auto sortedBack2 = vs::all(sv) | actions::sort([](auto a, auto b) { return a < b;}, &string::size); // OK
    auto sortedBack2 = vs::all(sv) | actions::sort(rg::greater(), &string::size);
    TraceX(sortedBack2);

//    auto sortedBack3 = vs::all(sv) | actions::sort(rg::greater(), &string::back); // Failed
    auto sortedBack3 = vs::all(sv) | actions::sort([](auto a, auto b) { return a.back() < b.back();}); // OK
    TraceX(sortedBack3);

    vector v = {3,1,2};
    vs::all(v) | actions::sort;
    cout << endl;
}

void ex_istream_view() {

	constexpr static auto input = "Niebloid comes from Eric Niebler's name. In simple words, they are function objects"
								  " that disable ADL (argument-dependent lookup)";

	auto words = vs::c_str(input) | views::split(' ');
	TraceX(words);
	TraceX(words | to<vector<string>>);
	auto words_size = (words | to<vector<string>>).size();
	TraceX(words_size);
	{
		istringstream text(input);
		auto v = rg::istream<string>(text); // = istream_view<string>(text);
		vector<string> vecText = v | to_vector;
		TraceX(v);
		TraceX(vecText);
	}

	auto v = streamView( istringstream(input) );
	auto sizeMax = rg::max(v, std::less{}, &string::size).size();
	TraceX(sizeMax);

	auto sv = streamView( istringstream(input) ) | to_vector;
	auto sorted = vs::all(sv) | actions::sort;
	TraceX(sorted);

	{
		istringstream text(input);
		auto max1 =
				rg::max(rg::istream_view<string>(text), [](const auto& a, const auto& b) { return a.size() < b.size(); });
        TraceX(max1);
//		auto max2 [[maybe_unused]] = rg::max(istream_view<string>(text), {}, &string::size); // range is empty; Assertion failed
//        auto wordsView = [](auto input){ istringstream text(input); return rg::istream<string>(text); };
// WTF       auto max2 [[maybe_unused]] = rg::max(wordsView(input), {}, &string::size); // range is empty; Assertion failed
//        TraceX(max2);
	}

	istringstream text(input);
	TraceX(istream_view<string>(text));
	TraceX(text.gcount(), text.eof());
	cout << endl;
}

void ex5() {
	constexpr static auto input = "Niebloid comes from Eric Niebler's name. In simple words, they are function objects"
								  " that disable ADL (argument-dependent lookup)";
	istringstream from(input);

	vector<string> num(8);
	rg::generate(num, [k=0]() mutable { return to_string(k++); });
	TraceX(num);

	vector<string> words;
	rg::copy(num, rg::back_inserter(words));
	TraceX(words);

	{
		istringstream from(input);
		TraceX(istream_view<string>(from));
	}

	words = rg::to_vector(istream_view<string>(from));
	words = istream_view<string>(from) | rg::to_vector;

	auto s = string(input);
	TraceX(s | vs::split(' '));
	TraceX(s | vs::split(' ') | to<vector<string>>);

	istringstream f2(input);
	TraceX(istream_view<string>(f2));

	istringstream f3(input);
	vector<string> v3 = istream_view<string>(f3) | to_vector;
	TraceX(v3);
//	  auto v2 = i2 | vs::split(' ') | to<vector<string>>);

//	  words = istream_view<string>(from) | rng::to<string>;

//	  vector<string> words = istream_iterator<string>(from);
//	  rng::copy(istream_iterator<string>(from), rng::back_inserter(words));
}

void ex6() {
	auto GetEvens =
		views::for_each(views::iota(1,9), [](int i) {
			return yield_if((i % 2) == 0, i);
		});
	TraceX(GetEvens);
}

void ex7() {
	using views::drop;
	using views::transform;

	auto const s = std::string{"feel_the_force"};
	auto words = s | views::split('_'); // [[f,e,e,l],[t,h,e],[f,o,r,c,e]]
	auto words_cap = words | views::transform([](auto w){
		auto head = w | views::take(1)
					  | views::transform([](unsigned char c){return std::toupper(c);}); // e.g. [F]
		return views::concat(head, w | views::tail); // e.g. [F,e,e,l]
	}); // [[F,e,e,l],[T,h,e],[F,o,r,c,e]]
	auto s_camelcase = words_cap | views::join | to<std::string>(); // FeelTheForce
	TraceX(s_camelcase);
}

void ex_71() {
	using views::take;
	using views::drop;
	using views::transform;

	auto firstUp = views::take(1) | transform(::toupper);
	auto title_view = [firstUp](auto w){
			return views::concat(w | firstUp, w | drop(1)); // e.g. [F,e,e,l]
		};
	auto words = "feel_the_force"sv | views::split('_'); // [[f,e,e,l],[t,h,e],[f,o,r,c,e]]
	auto words_cap = words | views::transform(title_view); // [[F,e,e,l],[T,h,e],[F,o,r,c,e]]
	auto s_camelcase = words_cap | views::join("<+>"sv) | to<std::string>(); // FeelTheForce
	TraceX(s_camelcase);
}

void ex_8() {
	std::string input = " 1a2/**/a3Z4b5Z6cz ";
	std::string result =
			input // OK
//			vs::c_str(" 1A2a3Z4b5Z6cz ") // ok
			| vs::filter(::isalpha)
			| vs::transform(::tolower)
			| to<std::string>
			| actions::sort
			| actions::unique;
	TraceX(result);
    cout << endl;
}

extern "C" void print_plainCStr(int argc, const char** argv) {
	for (int k=0; k<argc; ++k) {
		printf("print_plainCStr> %d: %s\n", k, argv[k]);
	}
}

// convert vector<string> to const char**
void test_cstr_array() {
	vector<string> args = {"one", "two", "three"};
//	auto argv = args | vs::transform([](const string& s){ return s.c_str(); }) | to_vector;
	auto argv = args | vs::transform(&string::c_str) | to_vector;
	print_plainCStr(argv.size(), &argv[0]);
}

using index_t = unsigned int;
using value_t = int;
extern "C" value_t partial_sum(index_t n) {
	auto s = (value_t)0;
	for (auto k = 0u; k <= n; ++k) { s += k; }
	return s;
}

extern "C" value_t fibonacci(index_t n)
{
	return n <= 1 ? (value_t)n
		: fibonacci(n-1) + fibonacci(n-2);
}

extern "C" index_t get_plainC() { return 8; }
void test_C_accessor(index_t (get_num)(), value_t (get_value)(index_t)) {
	auto v = vs::iota(0u, get_num()) | vs::transform([f = get_value](auto k) { return f(k); });
	TraceX(v.size(), v | vs::reverse);
}

void ex_stringView() {

}

int main() {
	auto _ = Scoped {
		[](){ log_trace_(main) << "Starting..."; },
		[](){ log_trace_(main) << "Done."; }
	};

	ex_slice();

	auto fmt = formatStr(16);
//	log_trace << fmt;

//	for (auto k : {0,1,2,3}) {	test_gen2(); TraceX(k); }

	for (const auto& x : test_gen2()) cout << x << ", "; cout << endl;

	ex_slice();
	cipherCaesar();
	test_foreach();
	test_toVector();
	test_shuffle();
	test_transform();
	test_temporary();

	ex_generate(10);

	ex_foreach();
	ex_istream_view();
//	ex5();
//	ex6();
//	ex7();
//	ex_71();
	ex_8();

//	test_cstr_array();
//	test_C_accessor([]{return 8u;}, fibonacci);
//	test_C_accessor(get_plainC, fibonacci);

    ex_wordTokenizer();
    ex_sort();

	return 0;
}