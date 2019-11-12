#include "../share/wmc_trace.h"
#include "../share/wmc_assert.h"

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <inttypes.h>
#include <algorithm>
#include <vector>
#include <iomanip>
#include <cassert>

#include <cstdio>

#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>

using namespace std;



//! Log str safely using mkstemp
/*
static void log_str0(const char* fname, const char* buf, int length)
{
    int n = strlen(fname);
    wmc_assert(n > 0) << "Illegal parameter: empty file name";
    std::vector<char> fn; fn.reserve(n + 8);
    fn.insert(fn.end(), fname, fname+n);
    fn.push_back('.');
    fn.insert(fn.end(), 6, 'X');
    fn.push_back(0);

    int fd = ::mkstemp(&fn[0]);
    wmc_require(fd > 0) << "Can not create temporary file '" << &fn[0] << "'" << ": " << strerror(errno);

    int written = ::write(fd, buf, length);
    wmc_require(written == length) << strerror(errno);

    wmc_require(::close(fd) == 0) << strerror(errno);
    wmc_require(::chmod(&fn[0], S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH) == 0) << strerror(errno);
    wmc_require(::rename(&fn[0], fname) == 0) << strerror(errno);
}
*/

int lock_file()
{
	const char* const lock_name = "x.txt.lock";
	const char* const fname = "x.txt";
	
//	int fd2;
//	const char* const lock2 = "x.txt.lock2";
//	while ((fd2 = open(lock2, O_WRONLY | O_CREAT | O_EXCL)) <= 0 ) { }
	
	int fd = 0;
	int lock_counter = 0;
	
	while ( (fd = open(lock_name, O_WRONLY | O_CREAT | O_EXCL)) <= 0 ) {
		
	//	if (++lock_counter > 1000) break;
	}
	if (lock_counter) {
		err_stream << "lock_counter = " << lock_counter;
		return -1;
	}
	
	errno = 0;	// not reliable due to multithreading
	
//	TraceX(fd);
	if (fd >= 0) {
		int n = 0;
		FILE* fp = fopen(fname, "r");
		wmc_require(fp > 0) << strerror(errno);	// len: ignore zero end byte

		wmc_require( ::fscanf(fp, "%d", &n) == 1 ) << strerror(errno);
		wmc_require( ::fclose(fp) == 0 ) << strerror(errno);
//	TraceX(n);
		
		n += 1;
		char	buf[64];
		int len = snprintf(buf, sizeof(buf), "%d\n", n);
		wmc_require(len >= 0) << strerror(errno);
//	TraceX(len);
		
		wmc_require(len < sizeof(buf)) << "Insufficient snprintf buffer size";	// '<' - ensure zero end byte
		wmc_require( write(fd, buf, len) >= 0) << strerror(errno);	// len: ignore zero end byte
//	TraceX("4:");
		
//		wmc_require(::close(fd) == 0) << strerror(errno);
//	TraceX("5:");
	//	wmc_require(::chmod(lock_name, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH) == 0) << strerror(errno);
	
		struct stat st;
		::stat(lock_name, &st);
		
		wmc_require(::chmod(lock_name, st.st_mode | S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH) == 0) << strerror(errno);
		wmc_require(::rename(lock_name, fname) == 0) << strerror(errno);

		::close(fd);
		
//	TraceX("6:");
	}
	
//	wmc_require(::close(fd2) == 0) << strerror(errno);
//	remove(lock2); // wmc_require(::remove(lock2) == 0) << strerror(errno);
	return 0;
}


void cleanup(const char* const lock_name) 
{
	errno = 0;
	wmc_require( ::remove(lock_name) == 0 || errno == ENOENT) << strerror(errno);
}

int main( void )
{
	tr_stream << "This is a test";

	try {
	//	int	n = 0;
	//	FILE* fp = fopen("x.txt", "r+");

// wrong idea:
//	cleanup("x.txt.lock");
		
		for (int k=0; k<100*1000; ++k) {
	/*
		//	err_stream << "1;";
			fseek(fp, 0, SEEK_SET);
		//	err_stream << "2;";
			fscanf(fp, "%d", &n);
		//	err_stream << "3; " << n;
			n += 1;
			fseek(fp, 0, SEEK_SET);
			fprintf(fp, "%d\n", n);
	*/
			lock_file();
		}
	}
    catch (exception& e) {
        log_error << " Exception occured | " << e.what();
    }
    catch (...) {
        log_error << " Unknown error occured";
    }
	
	tr_stream << "Test finished";
	return 0;
}


