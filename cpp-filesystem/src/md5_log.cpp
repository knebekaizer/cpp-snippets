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
#include <deque>
#include <algorithm>

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

#include "timing.hpp"
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
#include "gsl-lite.hpp"

uint128_t x2u128(string_view s) {
    static array<gsl::czstring, 160> fmt = []() {
        CTime gen;
        static string buf[160];
        generate(begin(buf), end(buf), [k = 0] () mutable {
            return string("%") + to_string(++k) + "xll";
        });
        array<gsl::czstring, fmt.size()> fmt;
        transform(begin(buf), end(buf), begin(fmt), [] (auto& it) {
            return it.c_str();
        });
        auto stop = gen();
        return fmt;
    }();

    using fmt_t = char[8];
    static fmt_t* fmt2 = []() {
        CTime gen;
        static fmt_t buf[160];
        for_each(begin(buf), end(buf), [k = 0] (auto p) mutable {
            snprintf(p, 8, "%%%dxll", ++k);
        });
        auto stop = gen();
        TraceX(stop, gen);
        return buf;
    }();
    static struct printme {
        printme() {
            for (auto x : gsl::make_span(fmt2, 16)) TraceX(x);
        }
    } _;

    uint64_t high = 0, low;
//    if (s.size() > 16) {
//        istringstream(string(s.substr(0, s.size() - 16))) >> hex >> high;
//    }
//    istringstream( string(s.substr(max(0, (int)s.size() - 16))) ) >> hex >> low;
//    cout << s << endl;
//    cout << hex << high << low << dec << "\n";

    int half = max(0, (int)s.size() - 16);
    if (half > 0) {
//        ostringstream fmt;
//        fmt << "%" << half << "xll";
//        TraceX(fmt.str());
        sscanf(&s[0], fmt[half], &high);
    } else {
        high = 0;
    }
//    ostringstream fmt;
//    fmt << "%" << (32 - half) << "xll";
    sscanf(&s[half], fmt[half], &low);
//    cout << hex << high << low << dec << "\n\n";

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


using Args = gsl::span<char const*>;

void options(const Args& args) {
    for (auto& x : args) { TraceX(x); }
    vector<string> s = {"a", "b"};
}

std::ostream& operator<<(std::ostream& out, const std::chrono::duration<double>& t) {
    return out << to_string(t.count()) << "s";
}

std::ostream& operator<<(std::ostream& out, const std::chrono::nanoseconds& t) {
    return out << to_string(t.count()) << "ns";
}

std::ostream& operator<<(std::ostream& out, const CTime& t) {
    return out << to_string(t.nsec().count()) << "ns";
}

struct File {
    uint128_t md5;
    string fname;
};

//using FList = vector<File>;
using FList = deque<File>;

void load(istream& inp) {
    CTime t;
    uint128_t md5;
    string fname;
    log_trace << "Using deque";
    FList flist;
    while (inp >> md5 && inp.ignore(2)
            && getline(inp, fname)) {
        flist.push_back({md5, fname});
//        cout << u128x(md5) << " -> " << fname << endl << flush;
    }
    TraceX(t.nsec(), t.sec());
}


int main(int argc, char const **argv) {
    TraceX(__clang_version__);

//    using Args = invoke_result(decltype( gsl::make_span(argv, argc)))::type;  //vector<string>;
//    using Args = result_of<decltype(gsl::make_span(argv, argc))>::type;  //vector<string>;

    auto args = gsl::make_span(argv, argc);
    options(args);


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