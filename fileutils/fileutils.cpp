#include "wmc_trace.h"

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <inttypes.h>
#include <algorithm>
#include <vector>
#include <iomanip>
#include <cassert>

using namespace std;

#include <sys/stat.h>
#include <fcntl.h>
//! Read entire file into the vector
template<typename T> 
vector<T>& readFile(const char* name, vector<T>& buf)
{
	// get file size
	struct stat file_info;
	int rc = stat(name, &file_info);
//	if (rc == -1) raise(runtime_error, errno) << 
	TraceX(rc);
	if (rc == -1) throw runtime_error("Can't stat");
	TraceX(file_info.st_size);

	int fd = open(name, O_RDONLY);
	if (fd == -1) throw runtime_error("Can't open");
	
	buf.resize(file_info.st_size);
	rc = read(fd, (char*)&buf[0], buf.size());
	
	close(fd);
	
	return buf;
}

template<typename T> 
vector<T>& readFile2(const char* name, vector<T>& buf)
{
	ifstream inp(name);
	inp.exceptions(ios::badbit | ios::failbit | ios::eofbit);
	
	inp.seekg(0, ios::end);
	int sz = inp.tellg();
	inp.seekg(0, ios::beg);

	TraceX(sz);
	buf.resize(sz);
	
	inp.read((char*)&buf[0], buf.size());
	
	return buf;
}


int main( void )
{
	tr_stream << "This is a test";

	try {	
		vector<char> buf;
		readFile("fileutils.cpp", buf);
		
		buf.push_back(0);
		cout << &buf[0];
	}
	catch (exception& e) {
		log_error << e.what() << "; errno = " << errno;
	}
	
	getchar();
	try {	
		vector<char> buf;
		readFile2("fileutils.cpp", buf);
		
		buf.push_back(0);
		cout << &buf[0];
	}
	catch (exception& e) {
		log_error << e.what() << "; errno = " << errno;
	}

	tr_stream << "Test finished";
	return 0;
}


