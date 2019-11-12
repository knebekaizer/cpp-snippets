#include "wmc_trace.h"

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <inttypes.h>
#include <algorithm>
#include <vector>

#include <sys/times.h>

using namespace std;

static pid_t sPid;


struct time_data {
    uint64_t    real_ms;
    uint64_t    user_ms;
    uint64_t    sys_ms;
};


class Clock : public time_data {
public:
    Clock() { reset(); }
    
    time_data elapsed() const { 
        tms     st_cpu;
        clock_t real_clock = times(&st_cpu);       
        time_data t = {real_clock - real_ms, st_cpu.tms_utime - user_ms, st_cpu.tms_stime - sys_ms};
        return t;
    }
    
    void reset() {  
        tms     st_cpu;
        real_ms = times(&st_cpu);
        user_ms = st_cpu.tms_utime;
        sys_ms = st_cpu.tms_stime;
    }
};


std::ostream& operator <<(std::ostream& os, time_data const& t) {
    return os 
    //  << "timeUSR=" 
        << t.user_ms
        << ":" << t.sys_ms
        << ":" << t.real_ms;
}


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
//	start_clock();
	
    tr_stream << "Sleep...";
	sleep(5);
    tr_stream << clock.elapsed();
	
	const size_t N = 10000000;
/*
	ofstream out("r.dat");
	for (size_t k=0; k!=N; ++k) {
		out << lrand() << '\t' << k << endl;
		if (k % 1000000 == 0) tr_stream << k / 1000000;
	}
*/
	tr_stream << "Fill...";
	vector<uint64_t> rand_vec;
	for (size_t k=0; k!=N; ++k) {
		rand_vec.push_back(lrand());
	}
	tr_stream << clock.elapsed();
	
	tr_stream << "Sort...";
	sort(rand_vec.begin(), rand_vec.end());
	
    tr_stream << clock.elapsed();
	tr_stream << "Done.";
	tr_stream << "Test finished";
	return 0;
}


