#include "wmc_trace.h"
#include "wmc_clock.h"

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <inttypes.h>
#include <algorithm>
#include <vector>

#include <sys/times.h>

using namespace std;
using wmc::Clock;
//using namespace wmc;

static pid_t sPid;


uint64_t lrand()
{
	const int RandMaxBitLength(31);
	uint64_t r = rand();
	r <<= RandMaxBitLength;
	r |= rand();
	return r;
}

int main( void )
{
	tr_stream << "This is a test";
	
	sPid = getpid();
	
	TraceX(RAND_MAX);
	
	TraceX(lrand());
	TraceX(lrand());
	
	TraceX(CLOCKS_PER_SEC);
	
	Clock clock;
	
    tr_stream << "Sleep...";
	sleep(5);
    tr_stream << clock.elapsed();
	
	const size_t N = 10000000;

	tr_stream << "Fill...";
	vector<uint64_t> rand_vec;
	for (size_t k=0; k!=N; ++k) {
		rand_vec.push_back(lrand());
	}
	tr_stream << clock.elapsed();
	
	tr_stream << "Sort...";
	sort(rand_vec.begin(), rand_vec.end());
	
    tr_stream << clock.elapsed();
	tr_stream << "Test finished";
	return 0;
}


