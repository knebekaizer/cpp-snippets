#include "share/wmc_trace.h"

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <inttypes.h>
#include <algorithm>
#include <vector>
// #include <stdlib.h>	mkstemp
#include <stdio.h>
#include <errno.h>
#include <stdexcept>

#include "wmc_clock.h"
using wmc::Clock;

using namespace std;

//static pid_t sPid;

namespace wmc {


	class runtime_error : public ::std::runtime_error {
	public:
		runtime_error(const string& where_arg, const string& what_arg) 
			: ::std::runtime_error(what_arg)
			, where_(where_arg) {}
			
		virtual ~runtime_error() throw() {};
			
		virtual const char* where() const throw() {return where_.c_str();}
	private:
		string where_;
	};

	class logic_error : public ::std::logic_error {
	public:
		logic_error(const string& where_arg, const string& what_arg) 
			: ::std::logic_error(what_arg)
			, where_(where_arg) {}

		virtual ~logic_error() throw() {};
		virtual const char* where() const throw() {return where_.c_str();}
	private:
		string where_;
	};


}

namespace Assertions {

template <class Exception>
class exc_helper
{
public:
    exc_helper(bool condition = false) : cond_(condition) {}

    std::ostringstream& get() const { return os_; }

    ~exc_helper() 
    {
    	/*
    	 *  -------------        THIS DTOR CAN THROW       ---------------------
    	 *	Yes I know: my mom told me "Please never-ever leak an exception from dtor"
    	 *
    	 *	See GOTW $& (http://www.gotw.ca/gotw/047.htm) for preliminary discussion
    	 *	Huge number of other sources is also available.
    	 
    	 * In this very particular case throwing from dtor is sane and quite OK.
    	 * 
    	 * Both technical and moral arguments from GOTW47 are not relevant here:
    	 *	- first (technical) objection is not applicable: using Failure or exc_helper
    	 * in a dtor should be considered harmful and no-no; if used inadvertently then 
    	 * disabling of actual throw would be a reasonable safety means.
    	 *  - second (moral) objection is not applicable because ~exc_helper dtor has no 
    	 * other semantics but throwing this exception.
    	 */
    	if (!cond_ && !std::uncaught_exception()) {
			throw Exception(os_.str());
    	}
    }
    
private:
    mutable std::ostringstream os_;
    const bool cond_;
    
    // Unintentional object copy will cause unwanted extra flush() operation.
    exc_helper(const exc_helper<Exception>&);
};

struct ELocationHelper {
	ELocationHelper(const char* file, int line, const char* func)
		: fn_(file), ln_(line), fu_(func) {}
	
	operator string() const {
		std::ostringstream os_;
		os_ << fn_ << "#" << ln_ <<"@"<< fu_;
		return os_.str();
	}
	
	string fn_;
	int	ln_;
	string fu_;
};


}

//typedef Assertions::exc_helper<std::runtime_error> Failure;
typedef Assertions::exc_helper<std::logic_error> Assert;
using Assertions::raise;

#define Failure(cond)	Assertions::exc_helper<std::runtime_error>(cond).get() << \
	__FILE__"#"<<__LINE__<<"@"<<__func__<<" | " << "Condition '"#cond"' failed> "

//#define Failure(cond)	Assertions::exc_helper<wmc::runtime_error>(cond) \
//	.where(Assertions::ELocationHelper(__FILE__, __LINE__, __func__)).get() << "Condition '"#cond"' failed> "



//! Log str safely using mkstemp
void log_str(const char* fname, const char* buf, int length) 
{
	char fn[] = "/tmp.XXXXXX";
	int fd = mkstemp(fn);
//	Failure(fd > 0) << "Can not create temporary file '" << fn << "'" << raise;
	Failure(fd > 0) << "Can not create temporary file '" << fn << "'";
	
	int written = write(fd, buf, length);
	Failure(written == length) << strerror(errno) << raise;
	
	close(fd);
	rename(fn, fname);
}

int main( void )
{
	tr_stream << "Starting the test";
	
	sPid = getpid();
	Clock clock;
	
	const char* const log_file = "base.log";


	try {
	
		// "Die hard" test 1: throws exception from within Failure context
		try {
			vector<int> v(9);
			Failure(v.size() > 9) << "Test case 1: this should never been caught!!!" << v.at(10);
		}
		catch (exception& e) {
			log_error << "Exception has been caught successfully | " << e.what();
		}
	
		// "Die hard" test 1: throws exception from within Failure context
		try {
			vector<int> v(9);
			Failure(v.at(10)) << "Test case 2: this should never been caught!!!";
		}
		catch (exception& e) {
			log_error << "Exception has been caught successfully | " << e.what();
		}
		
		
		for (int k=0; k!=10000; ++k) {
			char	buf[64];
			wmc::time_data t = clock.elapsed();
			int len = snprintf(buf, sizeof(buf), "%ld:%ld:%ld", t.user_ms, t.sys_ms, t.real_ms);
		
			log_str(log_file, buf, len);
		}	// 4:89:98
	} 
	catch (wmc::runtime_error& e) {
		log_error << " Exception occured | " << e.where() << " | " << e.what();
	}
	catch (exception& e) {
		log_error << " Exception occured | " << e.what();
	}


    tr_stream << clock.elapsed();
	tr_stream << "Done.";
	tr_stream << "Test finished";
	return 0;
}


