#include "share/wmc_trace.h"

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <inttypes.h>
#include <algorithm>
#include <vector>
#include <iterator>
#include <iomanip>

#include "checked.h"
#include "unchecked.h"

using namespace std;

struct QueryInfo {
	string name;
	int	x,y;
	int reg;
	int		shows;
	int 	showsWeight;
	int		clicks;
	int 	clicksWeight;
	string 	query;
};

istream& operator>>(istream& is, QueryInfo& q) {
	is >> q.name >> q.x >> q.y >> q.reg >> q.shows >> q.showsWeight
		>> q.clicks >> q.clicksWeight;
	is.ignore(1, '\t');
	if (is) getline(is, q.query);
	return is;
}


static void escapeChars(string& s) {
    char q = '\'';
    char esc = '\\';
    size_t pos = 0;
    while ((pos = s.find_first_of(q, pos)) != string::npos) {
        if (pos == 0 || s[pos-1] != esc) {
        	s.insert(s.begin() + pos, esc);
        	++pos;
        }
        ++pos;
    }
}

char const* asc = "qwerty";
string a[] = {
	"qwerty'", 
	"qwe'rty", 
	"'qwerty", 
	"asdf'''ghjkl",
	"asdf'\'\'ghjkl",
	"asdf\'\'''ghjkl",
	"''asdfghjkl",
	"a'sd'f\'\'''ghj'kl\'"
};


int main( void )
{

	tr_stream << "This is a test";

	TraceX(unsigned long(2) - unsigned long(-1));
//	TraceX(utf8::internal::is_trail(asc[3]));

	for (int k=0; k<sizeof(a)/sizeof(*a); ++k) {
		escapeChars(a[k]);
		TraceX(a[k]);
	}

	getchar();

	ifstream is("d.txt");
	QueryInfo q;
	try {
	while (is >> q) {
		TraceX(q.query.size());
		const int8_t* s = (const int8_t*)q.query.data();
		const int8_t* e = s + q.query.size();
		TraceX(utf8::is_valid(s, e));
		
		for (const int8_t* p=e; p>=s; --p) {
			TraceX(utf8::is_valid(p, e));
		}
getchar();
		while (e > s) {
			utf8::prior(e, s);
			TraceX(e - s);
		}
getchar();
		e = s + 255;
		while (e > s) {
			utf8::unchecked::prior(e);
			TraceX(e - s);
		}
		
		
break;
		const int8_t* z = e;
		while (z > s) {
			--z;
			if (*z < 0 && z-s < 256) break;
		}
		--z;
		TraceX(z-s);
		TraceX(utf8::is_valid(s, z));
		q.query.erase(z-s);
	//	TraceX(utf8::is_valid(s, z));
/*

		utf8::find_invalid(s, e);
		TraceX(e - s);
		const char* p = s;
		while (utf8::next(p, e)) {
			TraceX(p - s);
		}
		TraceX(p - s);
		tr_stream << "prior";
		if (e > s) {
			e -= 5;
			utf8::prior(e, s);
			TraceX(e - s);
		}
*/
		cout << endl;
	}
	} 
	catch (exception& e) {
		err_stream << "Exception handled: " << e.what();
	}
	tr_stream << "Test finished";
	return 0;
}


