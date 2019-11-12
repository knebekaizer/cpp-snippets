#ifndef WMC_TRACER_H_INCLUDED
#define WMC_TRACER_H_INCLUDED

#include <iosfwd>
#include <string>
#include <sstream>

namespace wmc
{


//***********************************************************************
// Tracing
//***********************************************************************

/*!
 *  FlashFunctor adds the log record taking string as a parameter
 *  It also takes care about EOL for text-stream based log
 */
template <typename FlushFunctor = std::ostream&>
class log_helper
{
public:
    explicit log_helper(FlushFunctor f) : flush(f) {}
    ~log_helper() { flush(os_.str()); }

    std::ostream& get() const { return os_; }
private:
    mutable std::ostringstream os_;
    FlushFunctor flush;
    // Unintentional object copy will cause unwanted extra flush() operation.
    log_helper(const log_helper<FlushFunctor>&);
};


//! log_helper specialization for std::ostream, eliminates extra buffering
template <>
class log_helper<std::ostream&>
{
public:
    explicit log_helper(std::ostream& os) : os_(os) {}
    ~log_helper() { os_ << std::endl; }

    std::ostream& get() { return os_; }
private:
    std::ostream& os_;
    log_helper(const log_helper<std::ostream&>&);
};


//! log_helper specialization for member function ptr ( the signature shall be 'void (T::*)(const std::string&)' ) 
template <class T>
class log_helper<void (T::*)(const std::string&)>
{
public:
    log_helper(T* p, void (T::*mf)(const std::string&)) : p_(p), mf_(mf) {}
    ~log_helper() { (p_->*mf_)(os_.str()); }

    std::ostream& get() { return os_; }
private:
    std::ostringstream os_;
    T*  p_;
    void (T::*mf_)(const std::string&);
    log_helper(const log_helper<std::ostream&>&);
};

/*!
 * Examples:
 * @code
 *  //  simple stdout tracer
 *  #define tr_stream   log_helper<>(std::cout).get() << __func__ << "> " 
 *
 *  //  specialized template, ctor with 2 parameters
 *  #define tr_stream   log_helper<PocoLogMethod>(logger, &Poco::Logger::error).get() << __func__ << "> " 
 *
 *  //  the same; non-specialized version using adapter class
 *  #define tr_stream   log_helper<PocoLogFunctor>(PocoLogFunctor(logger, &Poco::Logger::error)).get() << __func__ << "> " 
 *
 *  //  most generic mem_fun_ptr type. Take care if method is overloaded 
 *  #define tr_stream   log_helper< mem_fun_binder_t<void, Poco::Logger, const std::string&> >(mem_fun_binder(logger, &Poco::Logger::error)).get() << __func__ << "> " 
 * @endcode
 *
 */

//! Single-argument functor from member function pointer. Ctor accepts object pointer and method ptr
template <class S, class T, class A>
class mem_fun_binder_t
//  : public unary_function<A, S>
{
public:
    typedef S (T::*mem_t)(A);   // member function pointer type
    typedef T* obj_t;           // object type

    mem_fun_binder_t(T* obj, mem_t mf) : p_(obj), mf_(mf) {}
    S operator()(A x) const { return (p_->*mf_)(x); }
private:
    mem_t mf_;                  //!< member function pointer
    obj_t p_;                   //!< object pointer
};

// cp. std::mem_fun
template<class S, class T, class A>
inline
mem_fun_binder_t<S, T, A>
mem_fun_binder(T* obj, S (T::*mf)(A))
{
    return mem_fun_binder_t<S, T, A>(obj, mf);
}

/*!
 *  NullStream is an analogue of "null device"; use to disable output.
 * \code
 *    #define     tr_stream       papi::Nullstream()  
 * \endcode
 */
struct NullStream {
    template <class T> NullStream& operator<<(const T& ) { return *this; }
};


} // namespace wmc



// Defines are out of namespace anyway so use qualified typenames here
#ifdef NDEBUG
    #define tr_stream   wmc::NullStream()
    #define err_stream  wmc::log_helper<>(std::cerr).get() << "[ERROR]"
#else
    #define tr_stream   wmc::log_helper<>(std::cout).get() << __func__ << ":" << __LINE__ << "> " 
    #define err_stream  wmc::log_helper<>(std::cerr).get() << "[ERROR] "__FILE__"#"<<__LINE__<<"@"<<__func__<<"> "
#endif // NDEBUG

#define TraceF      tr_stream
#define TraceX(x)   tr_stream << #x" = " << x

#define log_trace   tr_stream
#define log_debug   tr_stream
#define log_info   tr_stream
//#define log_trace err_stream << [TRACE>
//#define log_debug err_stream << [DEBUG>
#define log_error   err_stream
#define log_fatal   err_stream



#endif // WMC_TRACER_H_INCLUDED
