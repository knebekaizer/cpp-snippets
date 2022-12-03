/**
 * \file
 * Trace and log functions and macros
 */

#pragma once

#include <stdexcept>
#include <string>
#include <iostream>
#include <sstream>
#include <iomanip>

#ifdef TRACE_PRETTY
//#define TRACE_FUNC TRACE_PRETTY_FUNCTION
#define TRACE_FUNC __PRETTY_FUNCTION__
#else
#define TRACE_FUNC __func__
#endif

namespace utils {

struct tr_stream_helper {
  std::ostream& get() {
    return std::cout;
  }
  ~tr_stream_helper() {
    std::cout << std::endl;
  }
};

struct err_stream_helper {
  std::ostream& get() {
    return std::cerr;
  }
  ~err_stream_helper() {
    std::cerr << std::endl;
  }
};


/*!
 *  NullStream is an analogue of "null device"; use to disable output.
 * \code
 *    #define     tr_stream       utils::NullStream()
 * \endcode
 */
class NullStream {
public:
  template <class T> NullStream& operator<<(const T& ) {
    return *this;
  }
};

inline std::string hexdump(const void* const buf, size_t len)
{
	std::ostringstream os;
	for (const uint8_t *p = (const uint8_t*)buf, *e = p + len; p != e; ++p) {
		os << std::setw(2) << std::setfill('0') << std::hex << (unsigned int)*p << ' ';
	}
	return os.str();
}

} // namespace utils

// Defines are out of namespace anyway so use qualified typenames here
//#ifdef NDEBUG
//#define tr_stream utils::NullStream()
//#define err_stream  utils::err_stream_helper().get()
//#else
#define tr_stream   utils::tr_stream_helper().get() << TRACE_FUNC << "> "
#define debug_stream   utils::tr_stream_helper().get() <<__FILE__<<":"<<__LINE__<<" :"<<TRACE_FUNC<<"> "
#define err_stream  utils::err_stream_helper().get() <<__FILE__<<":"<<__LINE__<<" :"<<TRACE_FUNC<<"> "
//#endif // NDEBUG

namespace LOG_LEVEL {
enum LOG_LEVEL {none = 0, fatal, error, warn, info, debug, trace, invalid};
}
//enum class LOG_LEVEL {none = 0, fatal, error, warn, info, debug, trace, invalid};

/*
 * Compilation: CXXFLAGS=-DDEF_LOG_LEVEL=2 make
 * or/and
 * CXXFLAGS=-DUSE_RUNTIME_LOG_LEVEL
 * and declare global variable as:
 * LOG_LEVEL::LOG_LEVEL gLogLevel = DEF_LOG_LEVEL;
*/
#ifndef DEF_LOG_LEVEL
#define DEF_LOG_LEVEL (LOG_LEVEL::trace)
#endif // DEF_LOG_LEVEL

#ifndef USE_RUNTIME_LOG_LEVEL
#define LOG_LEVEL_ DEF_LOG_LEVEL
#else   // USE_RUNTIME_LOG_LEVEL
extern LOG_LEVEL::LOG_LEVEL gLogLevel;
#define LOG_LEVEL_ gLogLevel
#endif  // USE_RUNTIME_LOG_LEVEL

#define log_trace   (LOG_LEVEL_ >= LOG_LEVEL::trace) && tr_stream
#define log_debug   (LOG_LEVEL_ >= LOG_LEVEL::debug) && debug_stream << "[DEBUG] "
#define log_info    (LOG_LEVEL_ >= LOG_LEVEL::info) && tr_stream << "[INFO] "
#define log_warn    (LOG_LEVEL_ >= LOG_LEVEL::warn) && debug_stream << "[WARN] "
#define log_error   (LOG_LEVEL_ >= LOG_LEVEL::error) && err_stream << "[ERROR] "
#define log_fatal   (LOG_LEVEL_ >= LOG_LEVEL::error) && err_stream << "[FATAL] "

#define TraceF      log_trace
#define Trace0()    TraceF
#define Trace1(a)   log_trace << #a << " = " << (a)
#define Trace2(a,b)   Trace1(a) <<"; " << #b << " = " << (b)
#define Trace3(a,b,c)   Trace2(a,b) <<"; " << #c << " = " << (c)
#define Trace4(a,b,c,d)   Trace3(a,b,c) <<"; " << #d << " = " << (d)
#define Trace5(a1,a2,a3,a4,a5)   Trace4(a1,a2,a3,a4) <<"; " << #a5 << " = " << (a5)


#define tr_stream1(name)  utils::tr_stream_helper().get() << #name << "> "
#define log_trace1(name)   (LOG_LEVEL_ >= LOG_LEVEL::trace) && tr_stream1(name)

// Variadic tricks
#if defined(__GNUC__) || defined(__clang__)
#define _TRACE_GET_MACRO(_0,_1,_2,_3,_4,_5,NAME,...) NAME
#define _TRACE_GET_MACRO1(_0,_1,NAME,...) NAME

#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wgnu-zero-variadic-macro-arguments"
#endif // clang diagnostic
#define TraceX(...) _TRACE_GET_MACRO(_0, ##__VA_ARGS__, Trace5, Trace4, Trace3, Trace2, Trace1, Trace0)(__VA_ARGS__)

// Use parameter instead of __func__. TODO: fix naming
#define tr_stream_(...) _TRACE_GET_MACRO1(_0, ##__VA_ARGS__, tr_stream1, tr_stream)(__VA_ARGS__)
#define log_trace_(...) _TRACE_GET_MACRO1(_0, ##__VA_ARGS__, log_trace1, log_trace)(__VA_ARGS__)

#if defined(__clang__)
#pragma clang diagnostic pop
#endif // clang diagnostic

#else
#define _TRACE_GET_MACRO(_1,_2,_3,_4,NAME,...) NAME
#define TraceX(...) _TRACE_GET_MACRO(__VA_ARGS__, Trace4, Trace3, Trace2, Trace1)(__VA_ARGS__)
#endif

//#undef LOG_ENABLED

#define NLOG4CXX
#ifndef NLOG4CXX
#include <log4cxx/logger.h>
class log_helper
{
public:
  explicit log_helper(log4cxx::LoggerPtr& l, const log4cxx::LevelPtr& lev)
    : logger(l), level(lev) {}
  ~log_helper() {
    logger->forcedLog(level, oss_.str(oss_), LOG4CXX_LOCATION);
  }

  std::ostream& get() {
    return oss_;
  }
private:
  ::log4cxx::helpers::MessageBuffer oss_;
  ::log4cxx::LoggerPtr logger;
  ::log4cxx::LevelPtr level;
  log_helper(const log_helper&);
};

extern log4cxx::LoggerPtr logger;

#undef tr_stream
#undef err_stream
#undef log_trace
#undef log_debug
#undef log_info
#undef log_warn
#undef log_error

//extern log4cxx::LoggerPtr logger;  // commented out. Instead, declare extern in the file where it needed as it may conflict with ather static declarations
#define tr_stream log_helper(logger, ::log4cxx::Level::getInfo()).get() << __func__ << "> "
#define err_stream log_helper(logger, ::log4cxx::Level::getError()).get() << __func__ << "> "
#define log_trace log_helper(logger, ::log4cxx::Level::getTrace()).get() << __func__ << "> "
#define log_debug log_helper(logger, ::log4cxx::Level::getDebug()).get() << __func__ << "> "
#define log_info log_helper(logger, ::log4cxx::Level::getInfo()).get() << __func__ << "> "
#define log_warn log_helper(logger, ::log4cxx::Level::getWarn()).get() << __func__ << "> "
#define log_error log_helper(logger, ::log4cxx::Level::getError()).get() << __func__ << "> "

#endif

