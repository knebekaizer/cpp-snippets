#include "wmc_trace.h"

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <inttypes.h>
#include <algorithm>
#include <vector>
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


int main( void )
{
	tr_stream << "This is a test";
	
	
	int x = 5;
	string s = "test";
	
	cout << "test"
		<< '\t'
		<< setw(3) << setfill('0') << 5
		<< '\t'
		<< setw(0) << 1
		<< endl;

	tr_stream << "Test finished";
	return 0;
}


