//
// Created by Vladimir Ivanov on 29.04.2021.
//

#ifndef CPP_FILESYSTEM_TIMING_HPP
#define CPP_FILESYSTEM_TIMING_HPP

//
// Created by Vladimir Ivanov on 29.04.2021.
//

#include <string>
#include <chrono>


std::string time() {
    static auto start = std::chrono::steady_clock::now();
    std::chrono::duration<double> d = std::chrono::steady_clock::now() - start;
    return "[" + std::to_string(d.count()) + "s]";
}

class CTime : std::chrono::steady_clock {
    time_point start = now();
public:
    using std::chrono::steady_clock::now;
    std::chrono::nanoseconds nsec(time_point t = now()) const { return t - start; }
    std::chrono::duration<double> sec(time_point t = now()) const { return t - start; }
    auto operator()() const { return nsec(); }
};

#include <iosfwd>
std::ostream& operator<<(std::ostream& out, const CTime&);
std::ostream& operator<<(std::ostream& out, const std::chrono::nanoseconds& sec);

#endif //CPP_FILESYSTEM_TIMING_HPP
