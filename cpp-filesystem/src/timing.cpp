//
// Created by Vladimir Ivanov on 29.04.2021.
//

#include <iostream>
#include <string>
#include <chrono>

#include "trace.h"

using namespace std::chrono;
using namespace std::chrono_literals;
using namespace std;

string time() {
    static auto start = std::chrono::steady_clock::now();
    std::chrono::duration<double> d = std::chrono::steady_clock::now() - start;
    return "[" + std::to_string(d.count()) + "s]";
}

class CTime : steady_clock{
    time_point start = std::chrono::steady_clock::now();
public:
    duration<double> sec() const { return now() - start; }
};