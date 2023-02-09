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
#include <map>
#include <algorithm>

#include <iostream>
#include <fstream>
#include <sstream>
#include <charconv>

#include <cstdint>
using int128_t = __int128;
using uint128_t = unsigned __int128;

#include <string_view>
#include <type_traits>

#include <limits>

//#include "gsl-lite.hpp"
#include "range/v3/all.hpp"
namespace rg = ::ranges;
namespace vs = ::ranges::views;

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

template <typename Func>
void once(Func f) {
    [[maybe_unused]] static auto _ = [&]() { f(); return '\0xff'; }();
//    static struct _{
//        _(Func f) { f(); }
//    }_(f);
}

void test_once() {
    auto lambda = [](){ cout << "Supposed to run only once\n"; };
    TraceX(sizeof(lambda));
    for (int k=0; k<4; ++k) {
        TraceX(k);
        once([](){ cout << "Supposed to run only once\n"; });
    }
}

uint128_t x2u128(string_view s) {
	uint128_t x;
	from_chars(s.begin(), s.end(), x, 16);
	return x;
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

struct Md5Record {
    uint128_t hash;
    string name;
};


inline std::istream& operator>>(std::istream& is, Md5Record& r) {
	string hash, name;
	if ((is >> hash) && isspace(is.get()) && getline(is, name)) {
		auto end = hash.data() + hash.size();
		uint128_t x = 0;
		auto rc = from_chars(hash.data(), end, x, 16);
		if (rc.ptr != end || rc.ec != std::errc{}) {
			is.setstate(is.rdstate() | ios::failbit);
			return is;
		};
		r.hash = x;
		r.name = std::move(name);
	}
	return is;
}

//using FList = vector<File>;
//using FList = deque<File>;

using Records = std::vector<Md5Record>;
using HashToName = std::multimap<uint128_t, string_view>;

void dedup(const HashToName& byHash);

void load(istream& ifs) {
	CTime t;

	auto iss = rg::istream<Md5Record>(ifs);
	Records buf = iss | rg::to_vector;
	if (!ifs.eof()) {
		log_error << "Invalid format at line #" << buf.size() + 1;
	}
	log_trace << "Total records: " << buf.size();

	auto byHash =
		buf
		| vs::transform([](const Md5Record& r) { return make_pair(r.hash, string_view(r.name)); })
		| rg::to<HashToName>;
	dedup(byHash);
}

void dedup(const HashToName& byHash) {

	auto byHashGroups = byHash
		| vs::chunk_by(
			[](const auto& x1, const auto& x2) {
				return x1.first == x2.first;
		});
	log_trace << "Total unique files: " << rg::distance(byHashGroups);
	size_t total = 0;
	size_t grpCount = 0;
	for (auto&& grp : byHashGroups) {
		int n = 0;
		for (auto& [hash, fn] : grp) {
			if (fs::exists(fn)) {
				if (auto sz = fs::file_size(fn); sz > 1024 * 64) {
					if (n == 1) ++grpCount;
					if (n++ > 0) total += sz;
				}
			}
		}
	}
	TraceX(grpCount, total>>20);
}


int main(int argc, char const **argv) {
    TraceX(__clang_version__);

	if (argc < 2) {
		log_error << "Usage " << argv[0] << " <file.md5>";
		return 1;
	}
	TraceX(argv[1]);
	ifstream ifs(argv[1]);
	if (!ifs) {
		log_error << "can't open file " << argv[1];
		return 2;
	}

	load(ifs);
	return 0;
}