#include "wmc_trace.h"

#include <iostream>

#include <errno.h>
#include <time.h>
#include <string.h>

using namespace std;
//#include <sys/time.h>


int main( void )
{
	tr_stream << "This is a test";

//	timeval tv;
//	// time_t         tv_sec      Seconds. 
//	// suseconds_t    tv_usec     Microseconds. 
//	
//	int rc = gettimeofday(&tv, 0);
//	// The gettimeofday() function shall return 0 and no value shall be reserved to indicate an error

	time_t sec = time(0);

	tm time;
	tm const* ret = localtime_r(&sec, &time);
	if (!ret) {
		err_stream << "localtime_r error: " << strerror(errno);
	}
	
	
	char buf[20] = {0};
	if (strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M.%S", &time)) {
    	tr_stream << buf;
    } else {
    	err_stream << "strftime error: probably no space left for output";
    }
    
	tr_stream << "Done.";
	tr_stream << "Test finished";
	return 0;
}


