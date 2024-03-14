#include "trace.hpp"
#include <iostream>
#include <typeinfo>
using namespace std;

void typeIDs() {
    TraceX(typeid(char const*).name());
    TraceX(typeid(char).name());
    TraceX(typeid(unsigned char).name());
    TraceX(typeid(signed char).name());
    TraceX(typeid(int8_t).name());
    TraceX(typeid(int16_t).name());
    TraceX(typeid(uint16_t).name());
    TraceX(typeid(int32_t).name());
    TraceX(typeid(uint32_t).name());
    TraceX(typeid(int64_t).name());
    TraceX(typeid(uint64_t).name());
    TraceX(sizeof(int), sizeof(long), sizeof(long long)); // 488 (64) or 448 (32)
}

void arithm_conversion() {
    typeIDs();
    TraceX(10 - 20u, typeid(10 - 20u).name());  // 4294967286
    TraceX(10 - 20u + 5l, typeid(10 - 20u + 5l).name());    // 4294967291
    TraceX(5l + 10 - 20u, typeid(5l + 10 - 20u).name());    // -5
    cout << boolalpha;
    TraceX((5l + 10 - 20u) == (10 - 20u + 5l)); // platform-dependent: false (-m64) or true (-m32)
    TraceX((5ll + 10l - 20ul) == (10l - 20ul + 5ll)); // platform-dependent: true (-m64) or false(-m32),
    cout << noboolalpha;
    TraceX(10l - 20u, typeid(10l - 20u).name()); // -10
    TraceX(10ll - 20u, typeid(10ll - 20u).name()); // -10
    TraceX(10l - 20ul, typeid(10l - 20ul).name()); // 18446744073709551606
    TraceX(-(10l - 20ul), typeid(-(10l - 20ul)).name()); // 10
    TraceX(-10 + 20ul, typeid(-10 + 20ul).name()); // 10
    TraceX(-20 + 10ul, typeid(-20 + 10ul).name()); // 10
}

int main() {
    arithm_conversion();
    return 0;
}