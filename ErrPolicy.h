#ifndef _ErrPolicy_h_
#define _ErrPolicy_h_

#include <stdexcept>
#include <string>
#include <iostream>

namespace cbr {

#ifdef NDEBUG
#define REQUIRE require
#define DEBUGMSG 
#else
#define REQUIRE(condition, msg) \
	if (!(condition)) { err_stream << "Condition '" #condition "' failed: " << (msg);  \
	signalError(msg); }

#define DEBUGMSG(condition, msg) \
	if (!(condition)) tr_stream << "Condition '" #condition "' failed: " << (msg)
#endif

template <class T>
class Error : public std::runtime_error
{
  public:
	explicit Error(const std::string& what_arg) : std::runtime_error(what_arg) {}
};


template <class T>
class ErrPolicy
{
  public:
	static void signalError(const std::string& s)	{ throw Error<T>(s); }
	
	static void require(bool condition, const std::string& faultMsg = "") {
		if (!condition) signalError(faultMsg);
	}

  protected:
	~ErrPolicy() {}		// don't delete me by non-virtual dtor
};


// global functions

#define signalError_(T) signalError((T*)0)

/*!
 * This template helps to separate compilation unit and decrease compilation time
 * Instead of including all classes definitions you may declare specialization 
 * for your bla::bla::SomeError using only forward declaration 
 * and put specialized implementation into separate .cpp file.
 * However, if you are not bothered by such separation just include you SomeError declaration
 * and invoke this template function - it will be instantiated for SomeError.
 * Example:
 */
template <class Error>
void signalError(const Error*)	{ throw Error(); }


/* **********************************************************************
 * Tracing
 * *********************************************************************/

struct tr_stream_helper {
	std::ostream& get() { return std::cout; }
	~tr_stream_helper() { std::cout << std::endl; }
};

struct err_stream_helper {
	std::ostream& get() { return std::cerr; }
	~err_stream_helper() { std::cerr << std::endl; }
};


/*!
 *  NullStream is an analogue of "null device"; use to disable output.
 * \code
 *    #define     tr_stream       papi::Nullstream()  
 * \endcode
 */
class NullStream {
public:
    template <class T> NullStream& operator<<(const T& ) { return *this; }
};

} // namespace cbr

// Defines are out of namespace anyway so use qualified typenames here


#if NDEBUG
	#define	tr_stream	cbr::NullStream()
	#define err_stream	cbr::err_stream_helper().get()
#else
	#define tr_stream   cbr::tr_stream_helper().get() << __func__ << ": "
	#define err_stream	cbr::err_stream_helper().get() << "[ERROR "__FILE__"#"<<__LINE__<<":"<<__func__<<"] "
#endif // NDEBUG

#define TraceF      tr_stream << "<"
#define TraceX(x)   tr_stream << #x << " = " << x



#endif
