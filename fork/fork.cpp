#include "../wmc_trace.h"

#include <unistd.h>

#include <iostream>

using namespace std;


int main( void )
{
	log_trace << "This is a test";
	
    pid_t pid = fork();

    string s;
    while (cin >> s) {
        cout << pid << ": " << s << endl;
    }
    
	log_trace << pid << "Test finished";
	return 0;
}


