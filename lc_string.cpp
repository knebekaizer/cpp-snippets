#include "share/wmc_trace.h"

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <inttypes.h>
#include <algorithm>
#include <vector>
#include <iterator>
#include <iomanip>

using namespace std;

/*
void test(istream const& inp)
{
	istream input(inp);
    while (input.good()) {
    	string s;
    	input >> s;
    	cout << s;
    }

}
*/

/*
class lc_iterator
	: public iterator<input_iterator_tag, char>
//	: public string::const_iterator
{
	string::const_iterator it_;
	char	value_;
public:
	lc_iterator() {}
	lc_iterator(lc_iterator& i) : it_(i.it_) {}
//	explicit lc_iterator(string::iterator i) : it_(i) {}
	lc_iterator(string::const_iterator i) : it_(i) {}

	const char  operator*() const { return tolower(*it_); }
	const char operator->() const   { return tolower(*it_); }
};
*/

//! lowcase char input iterator
class lc_iterator
	: public string::const_iterator
{
//	mutable char	v_;
public:
	lc_iterator() {}
	lc_iterator(lc_iterator& i) : string::const_iterator(i) {}
	lc_iterator(string::const_iterator const& i) : string::const_iterator(i) {}
	lc_iterator(string::iterator const& i) : string::const_iterator(i) {}

	const char  operator*() const { return tolower(string::const_iterator::operator*()); }
//	const char* operator->() const   { v_ = **this; return &v_; }
};


int main( void )
{
	tr_stream << "This is a test";

	string s;
	string lc;
	while (cin >> s) {
		lc.resize(s.size());
		transform(s.begin(), s.end(), lc.begin(), (int(*)(int))tolower);
		tr_stream << lc;
		
		string lc2(lc_iterator(s.begin()), lc_iterator(s.end()));
		TraceX(lc2.size());
		TraceX(lc2);
		for (lc_iterator i = s.begin(); i!=s.end(); ++i) {
			cout << *i;
		}
		cout << endl;
	}
	
	tr_stream << "Test finished";
	return 0;
}


