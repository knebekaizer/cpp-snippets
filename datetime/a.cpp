#include "wmc_trace.h"

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <unistd.h>

#include <sys/times.h>

using namespace std;

static pid_t sPid;


int main( void )
{
	tr_stream << "This is a test";

	ifstream ifs("x");
	int x;
	while ( ifs >> x) {
		TraceX(x);
		sleep(5);
	}

	tr_stream << "Test finished";
	return 0;
}


