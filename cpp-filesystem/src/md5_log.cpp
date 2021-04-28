//
// Created by Vladimir Ivanov on 28.04.2021.
//

//
// Created by Vladimir Ivanov on 22/10/2019.
//


//#include <filesystem>
#include "ghc/filesystem.hpp"
namespace fs = ghc::filesystem;
//namespace fs = std::__fs::filesystem;
//namespace fs = std::filesystem;

#include <exception>
#include <string>
#include <cassert>
#include <vector>

#include <iostream>
#include <fstream>
#include <sstream>


#include <cstdint>
using int128_t = __int128;
using uint128_t = unsigned __int128;

#include <string_view>
#include <type_traits>

#include <limits>

#include "gsl-lite.hpp"

#include "trace.h"

using namespace std;

/* md5:
5e1463677a8246bf2f99ea81f6baf1a6
123456789 123456789 123456789 12
18446744073709551615 // std::numeric_limits<uint64_t>::max()
 */
static string testStr = "5e1463677a8246bf2f99ea81f6baf1a6  Music/BG/Okudjava/15_track.mp3";

std::ostream&
operator<<( std::ostream& dest, __uint128_t value )
{
    std::ostream::sentry s( dest );
    if ( s ) {
        char buffer[ 128 ];
        char* d = std::end( buffer );
        do {
            -- d;
            *d = "0123456789"[ value % 10 ];
            value /= 10;
        } while ( value != 0 );
        int len = std::end( buffer ) - d;
        if ( dest.rdbuf()->sputn( d, len ) != len ) {
            dest.setstate( std::ios_base::badbit );
        }
    }
    return dest;
}

std::ostream&
operator<<( std::ostream& out, __int128_t value )
{
    if (value < 0) {
        out << '-' << (__uint128_t)(-value);
    } else {
        out << (__uint128_t)value;
    }
    return out;
}

extern "C" __int128 atoi128(const char *s);

uint128_t x2u128(string_view s) {
    uint64_t high = 0, low;
    if (s.size() > 16) {
        istringstream(string(s.substr(0, s.size() - 16))) >> hex >> high;
    }
    istringstream( string(s.substr(max(0, (int)s.size() - 16))) ) >> hex >> low;
//    cout << s << endl;
//    cout << hex << high << low << dec << "\n";
    return ((uint128_t)high << 64) + (uint128_t)low;
}

string u128x(uint128_t x) {
    ostringstream s;
    s << std::hex << (uint64_t)(x >> 64) << (uint64_t)(x & 0xffffffffffffffff);
    return s.str();
}

std::istream&
operator>>(std::istream& inp, uint128_t& v) {
    string s;
    if (inp >> s)
        v = x2u128(s);
    return inp;
}

std::istream&
operator>>(std::istream& inp, __int128_t& v) {
    string s;
    if (inp >> s) {
        v = atoi128(s.c_str());
    }
    return inp;
}

//constexpr string_view trim(string_view v, char c = '"') {
//    v.remove_prefix(std::min(v.find_first_not_of("\""), v.size()));
//    return v;
//}

void printTree(fs::path f, string indent = "") {
    cout << indent << f.filename().string() << (fs::is_directory(f) ? "/" : "") << endl;
    if (fs::is_directory(f))
        for (auto& child : fs::directory_iterator(f))
            printTree(child, indent + "    ");
}

void parseDir(const string& dirname)
{
	fs::path dir(dirname);
	assert(fs::is_directory(dir));
	fs::create_directories("sandbox/a/b");
	std::ofstream("sandbox/a/file1.txt");
	std::ofstream("sandbox/file1.txt");
	std::ofstream("sandbox/file2.txt");
    printTree("sandbox");
}

using Args = gsl::span<char const*>;

void options(const Args& args) {
    for (auto& x : args) { TraceX(x); }
    vector<string> s = {"a", "b"};
}

void load(istream& inp) {
    uint128_t md5;
    string fname;
    while (inp >> md5 && inp.ignore(2)
            && getline(inp, fname)) {
        cout << u128x(md5) << " -> " << fname << endl << flush;
    }
}


int main(int argc, char const **argv) {
    TraceX(__clang_version__);

//    using Args = invoke_result(decltype( gsl::make_span(argv, argc)))::type;  //vector<string>;
//    using Args = result_of<decltype(gsl::make_span(argv, argc))>::type;  //vector<string>;

    auto args = gsl::make_span(argv, argc);
    options(args);

//    TraceX(sizeof(intmax_t));
//    TraceX(sizeof(uint128_t));
//    TraceX(std::numeric_limits<uint64_t>::max());
//    cout << (int128_t)-1 << endl;
//    TraceX(std::numeric_limits<uint128_t>::max());
//    TraceX(std::numeric_limits<uint128_t>::min());
//    TraceX(std::numeric_limits<int128_t>::max());
//    TraceX(std::numeric_limits<int128_t>::min());

    try {
        if (args.size() > 1) {
            ifstream inp(args[1]);
            if (!inp) log_error << "Can't open " << args[1];
            load(inp);
        } else {
            load(cin);
        }
    }
    catch (std::exception& e) {
        log_error << e.what();
    }


    return 0;
}