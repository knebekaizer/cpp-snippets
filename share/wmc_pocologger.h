#ifndef WMC_POCOLOGGER_H_INCLUDED
#define WMC_POCOLOGGER_H_INCLUDED


#include <string>
#include "wmconsole/offline/WMCGet/wmc_trace.h"

// Poco::Logger support
#include <Poco/Logger.h>

namespace wmc
{

typedef mem_fun_binder_t<void, Poco::Logger, const std::string&>   PocoLogFunctor;
typedef PocoLogFunctor::mem_t                                      PocoLogMethod;

} // namespace wmc

//using wmc::mem_fun_binder_t;
//using wmc::log_helper;
using wmc::PocoLogFunctor;
using wmc::PocoLogMethod;

// ok : ctor with one parameter
//#define tr_stream log_helper<PocoLogFunctor>(PocoLogFunctor(logger, &Poco::Logger::error)).get() << __func__ << "> " 
// ok : most generic mem_fun_ptr type. Take care if method is overloaded 
//#define tr_stream log_helper< mem_fun_binder_t<void, Poco::Logger, const string&> >(mem_fun_binder(logger, &Poco::Logger::error)).get() << __func__ << "> " 
// ok : specialized template, ctor with 2 parameters
//#define TRACE_LOCATION    __FILE__"#"<<__LINE__<<":"<<__func__<<"> "
//#define TRACE_LOCATION    __func__ << "> "
#undef log_trace
#undef log_debug
#undef log_info
#undef log_warn
#undef log_error
#undef log_fatal
#define POCO_LOGGER &logger()
#define log_trace   wmc::log_helper<PocoLogMethod>(POCO_LOGGER, &Poco::Logger::trace).get() 
#define log_debug   wmc::log_helper<PocoLogMethod>(POCO_LOGGER, &Poco::Logger::debug).get() 
#define log_info    wmc::log_helper<PocoLogMethod>(POCO_LOGGER, &Poco::Logger::information).get() 
#define log_warn    wmc::log_helper<PocoLogMethod>(POCO_LOGGER, &Poco::Logger::warning).get() 
#define log_error   wmc::log_helper<PocoLogMethod>(POCO_LOGGER, &Poco::Logger::error).get() 
#define log_fatal   wmc::log_helper<PocoLogMethod>(POCO_LOGGER, &Poco::Logger::fatal).get() 
#define log_stat    log_info


#endif // WMC_POCOLOGGER_H_INCLUDED
