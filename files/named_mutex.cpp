#include "../share/wmc_trace.h"
#include "../share/wmc_assert.h"

#include <string>
#include <cstdio>

#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/sem.h>

using namespace std;

class FileLock {
	int _fd;
	string _fname;
	string _lock_name;
	
public:
	FileLock(const char* name) 
		: _fd(-1)
		, _fname(name)
	{
		_lock_name = ".lock.";
		_lock_name += _fname;
	}
	
	~FileLock() {
		if (_fd >= 0) force_cleanup();
	}
	
	void force_cleanup() {
		if (_fd < 0) return;
		::close(_fd);	// this may fail
		if (_lock_name.size()) ::remove(_lock_name.c_str());
		_fd = -1;
	}
	
	void lock() {
		int fd = -1;
		do {
			fd = open(_lock_name.c_str()
				, O_WRONLY | O_CREAT // | O_EXCL 
			);
		} while (fd < 0);
		_fd = fd;
	}
	
	void update() {
		errno = 0;
	//	struct stat st;
	//	::stat(_lock_name, &st);
		
//		wmc_require(::chmod(_lock_name, st.st_mode | S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH) == 0) << strerror(errno);
		wmc_require(::chmod(_lock_name.c_str(), S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH) == 0) << strerror(errno);
		wmc_require(::rename(_lock_name.c_str(), _fname.c_str()) == 0) << strerror(errno);
		::close(_fd);
		_fd = -1;
	}
	
	int get() const { return _fd; }
	

private:
	FileLock();
	FileLock(const FileLock&);
	FileLock& operator = (const FileLock&);
	
};

struct ScopeFileLock {
	FileLock	flock;
	ScopeFileLock(const char* name) : flock(name) { flock.lock(); }
	~ScopeFileLock() { 
		try {
			flock.update(); 
		} 
		catch (exception& e) {
			log_error << " Exception occured (FileLock.update failed): " << e.what();
		}
		catch (...) {
			log_error << " Unknown error occured (FileLock.update failed)";
		}
	}
	int get() const { return flock.get(); }
};



class NamedMutex {
	int _semId;
public:
	NamedMutex(const char* const name) 
		: _semId(create(name)) {}
	void lock() const  { lock(_semId); }
	void unlock() const { unlock(_semId); }
	
	static int create(const char* fileName);
	static void lock(int semid);
	static void unlock(int semid);
};

int NamedMutex::create(const char* fileName)	// static
{
	int fd = open(fileName, O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
	wmc_require(fd >= 0) << "cannot create named mutex (lockfile open() faliled): " << fileName;
	close(fd);

	key_t key = ftok(fileName, 0);
	wmc_require(key != (key_t)-1) << "cannot create named mutex (ftok() failed): " << fileName;
		
	int _semid = semget(key, 1, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH | IPC_CREAT | IPC_EXCL);
	if (_semid >= 0) {
		union semun arg;
		arg.val = 1;
		semctl(_semid, 0, SETVAL, arg);
	} else if (errno == EEXIST) {
		_semid = semget(key, 1, 0);
	} else throw runtime_error("cannot create named mutex (semget() failed)");
	
	return _semid;
}

/*
NamedMutexImpl::~NamedMutexImpl()
{
#if defined(sun) || defined(__APPLE__) || defined(__osf__) || defined(__QNX__)
//	sem_close(_sem);
#endif
}
*/


void NamedMutex::lock(int _semid)	// static
{
	struct sembuf op;
	op.sem_num = 0;
	op.sem_op  = -1;
	op.sem_flg = SEM_UNDO;
	
	int err;
	do {
		err = semop(_semid, &op, 1);
	} while (err && errno == EINTR);
	
	if (err) throw runtime_error("cannot lock named mutex");
}



void NamedMutex::unlock(int _semid)		// static
{
	struct sembuf op;
	op.sem_num = 0;
	op.sem_op  = 1;
	op.sem_flg = SEM_UNDO;
	if (semop(_semid, &op, 1) != 0) {
	   	throw runtime_error("cannot unlock named mutex");
	}
}


template <class Mutex>
class ScopedLock {
	Mutex& _mx;
public:
	ScopedLock(Mutex& m) : _mx(m) { _mx.lock(); }
	~ScopedLock() {
		try {
			_mx.unlock(); 
		}
		catch (exception& e) {
			log_error << " Exception occured (unlock failed): " << e.what();
		}
		catch (...) {
			log_error << " Unknown error occured (unlock failed)";
		}

	}
private:
	ScopedLock();
	ScopedLock(const ScopedLock&);
	ScopedLock& operator = (const ScopedLock&);
};


// ==================================================================================
// TEST
// ==================================================================================
#ifdef _TEST_LOCK_

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <inttypes.h>
#include <algorithm>
#include <vector>
#include <iomanip>
#include <cassert>

struct LogStat {
	int getHostInfo;
	int getMainMirror;
	int getSegment;
	int uploadHostData;
	int checkFilters;
};

ostream& operator<<(ostream& os, const LogStat& x) 
{
	return os 
		<< "getHostInfo\t"    << x.getHostInfo << "\n"
		<< "getMainMirror\t"  << x.getMainMirror << "\n"
		<< "getSegment\t"     << x.getSegment << "\n"
		<< "uploadHostData\t" << x.uploadHostData << "\n"
		<< "checkFilters\t"   << x.checkFilters << "\n"
	;
}

istream& operator>>(istream& is, LogStat& x) 
{
	string key;
	int value;
	while (is >> key) {
		if      (key == "getHostInfo"    && (is >> value)) x.getHostInfo    = value;
		else if (key == "getMainMirror"  && (is >> value)) x.getMainMirror  = value;
		else if (key == "getSegment"     && (is >> value)) x.getSegment     = value;
		else if (key == "uploadHostData" && (is >> value)) x.uploadHostData = value;
		else if (key == "checkFilters"   && (is >> value)) x.checkFilters   = value;
	}
	return is;
	/*
	while (getline(is, line)) {
		std::istringstream iss(line);
		string key;
		int value;
		if (iss >> key >> value) {
			if (key == "getHostInfo"
		}
	}
*/
}


template <class T>
void increment(T& st, int T::*mf) 
{
	st.*mf += 1;
}


int logIncr(const char* fname, int LogStat::*mf)
{
	ScopeFileLock fl(fname);
	
	
	errno = 0;	// not reliable due to multithreading
	int fd = fl.get();
	
	if (fd >= 0) {
		int n = 0;
		LogStat stat = {0};
		{
			ifstream fp(fname);
			fp >> stat; 	// safe if the file don't exist or incomplete
		}
		
		increment<LogStat>(stat, mf);
		
		std::ostringstream oss;
		oss << stat;
		const string& buf = oss.str();
		wmc_require( write(fd, buf.data(), buf.size()) >= 0) << strerror(errno);	// len: ignore zero end byte
	}
	
	return 0;
}


void* run(const char* name)
{
	tr_stream << "This is a test";

	try {
	//	int	n = 0;
	//	FILE* fp = fopen("y.txt", "r+");

// wrong idea:
//	cleanup("y.txt.lock");

	//	int _semid = create("y.txt.mx");
		string mx_name(name);
		mx_name += ".mx";
		NamedMutex mx(mx_name.c_str());
		
		for (int k=0; k<100*1000; ++k) {

			ScopedLock<NamedMutex> lock(mx);
		//	lock_file("y.txt");
		//	logIncr("y.txt");
			logIncr("y.txt", &LogStat::getSegment);
			logIncr("y.txt", &LogStat::checkFilters);
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


typedef void* (*fct)(void*);

int main( void )
{
	pthread_t tt[4]; 
	const char* name = "y.txt";
	
	for (int k=0; k<sizeof(tt)/sizeof(*tt); ++k) {
		pthread_create(&tt[k], 0, (fct)run, (void*)name);
	}
	for (int k=0; k<sizeof(tt)/sizeof(*tt); ++k) {
		pthread_join(tt[k], 0);
	}
	
	return 0;
}

#endif // _TEST_LOCK_
