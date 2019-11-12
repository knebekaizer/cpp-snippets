#include "../share/wmc_trace.h"

#include <unistd.h>

#include <iostream>
#include <fstream>
#include <vector>
#include <inttypes.h>

#include <sys/mman.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

#define TraceErr(x)	(x) && (tr_stream << strerror(errno)); 

using namespace std;


struct T {
	uint64_t	i;
	uint64_t	r;
};

ostream& 
operator << (ostream& os, const T& t) {
	return os << t.i << '\t' << t.r;
}

int test_write()
{
	log_trace << "1";
	
	vector<T> v(20);
	for (int k=0; k!=v.size(); ++k) {
		v[k].i = k;
		v[k].r = k%2;
	}
    
    ofstream	ofs("bin_test", ios::binary);
    ofs.write((const char*)&v[0], v.size() * sizeof(v[0]));
    
	log_trace << "done.";
	return 0;
}

void mm_read()
{
	int rc;
	int fd = open("bin_test", O_RDONLY);
	TraceErr(fd < 0);
	
	size_t	len = 20 * sizeof(T);
	void *p = mmap(0, len, PROT_READ, 0, fd, 0);
	TraceErr(p == MAP_FAILED);
	
	T* t = (T*)p;
	TraceX((intptr_t)p);
	for (size_t k=0; k!=len/sizeof(T); ++k) {
		tr_stream << t[k];
	}

	rc = munmap(p, len); TraceErr(rc);
	close(fd);
}

void mm_write()
{
	int rc;
	int fd = open("bin_test", O_RDWR);
	TraceErr(fd < 0);
	
	size_t	len = 20 * sizeof(T);
	void *p = mmap(0, len, PROT_READ | PROT_WRITE, 0, fd, 0);
	TraceErr(p == MAP_FAILED);
	
	T* t = (T*)p;
	for (size_t k=0; k!=len/sizeof(T); ++k) {
		t[k].r = t[k].i % 3;
		tr_stream << t[k];
	}

	rc = munmap(p, len); TraceErr(rc);
	close(fd);
}


int main()
{
	test_write();
	mm_read();
	mm_write();
}
