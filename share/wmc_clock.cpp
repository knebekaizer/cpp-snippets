#include "wmc_clock.h"
//#include "wmc_trace.h"

//#include <cstdlib>
#include <iostream>
#include <sys/times.h>
#include <unistd.h>

namespace wmc {
    
const unsigned int Clock::clocksPerSecond = sysconf(_SC_CLK_TCK);

static clock_t times_ms(tms& st_cpu) {
    clock_t t = times(&st_cpu);
    st_cpu.tms_utime = st_cpu.tms_utime * 1000 / Clock::clocksPerSecond;
    st_cpu.tms_stime = st_cpu.tms_stime * 1000 / Clock::clocksPerSecond;
    st_cpu.tms_cutime = st_cpu.tms_cutime * 1000 / Clock::clocksPerSecond;     // user time, children
    st_cpu.tms_cstime = st_cpu.tms_cstime * 1000 / Clock::clocksPerSecond;     // system time, children
    t = t * 1000 / Clock::clocksPerSecond;
    return t;
}

time_data 
Clock::elapsed() const 
{ 
    tms     st_cpu;
    clock_t real_clock = times_ms(st_cpu);
    return time_data(st_cpu.tms_utime - user_ms, st_cpu.tms_stime - sys_ms, real_clock - real_ms);
}
    
void 
Clock::reset() {  
    tms     st_cpu;
    real_ms = times_ms(st_cpu);
    user_ms = st_cpu.tms_utime;
    sys_ms = st_cpu.tms_stime;
}

} // namespace

std::ostream& operator <<(std::ostream& os, wmc::time_data const& t) {
    return os 
    //  << "timeUSR=" 
        << t.user_ms
        << ":" << t.sys_ms
        << ":" << t.real_ms;
}



