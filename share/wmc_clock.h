#ifndef WMC_CLOCK_H_INCLUDED
#define WMC_CLOCK_H_INCLUDED

#include <iosfwd>
#include <inttypes.h>

namespace wmc {

struct time_data {
    uint64_t    user_ms;
    uint64_t    sys_ms;
    uint64_t    real_ms;
    
    time_data(uint64_t u, uint64_t s, uint64_t r) // in milliseconds
        : user_ms(u)
        , sys_ms(s)
        , real_ms(r) {}
    
    time_data()
        : user_ms(0)
        , sys_ms(0)
        , real_ms(0) {}
    
    time_data& operator-=(const time_data& other) {
        user_ms -= other.user_ms;
        sys_ms -= other.sys_ms;
        real_ms -= other.real_ms;
        return *this;
    }
    
    time_data& operator*=(float d) {
        user_ms *= d;
        sys_ms *= d;
        real_ms *= d;
        return *this;
    }
};

inline
time_data operator-(const time_data& one, const time_data& other) 
{
    time_data t(one);
    t -= other;
    return t;
}

inline
time_data operator*(const time_data& one, float d) 
{
    time_data t(one);
    t *= d;
    return t;
}

class Clock : public time_data {
public:
    Clock() { reset(); }
    
    time_data elapsed() const;
    void reset();
    static const unsigned int clocksPerSecond;
};

} // namespace wmc


std::ostream& operator <<(std::ostream& os, wmc::time_data const& t);

#endif // WMC_TRACE_H_INCLUDED
