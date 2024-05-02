#include "trace.hpp"
#include <iostream>
#include <typeinfo>

#include <bitset>
#include <limits>
#include <climits>
#include <arpa/inet.h>

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

void bit_repr() {
    TraceX(numeric_limits<int>::digits);
    TraceX(numeric_limits<unsigned>::digits);
//    auto bits = [](auto x){ return bitset<numeric_limits<decltype(x)>::digits + 1>(x).to_string(); };
    auto bits = [](auto x){ return bitset<sizeof(x) * CHAR_BIT>(x).to_string(); };
    int8_t i = -128; // 128
    uint32_t j = i;
    int32_t ij = j;
    int8_t ii = j;
    TraceX((int)i, j, ij, (int)ii);
    TraceX(bits(i), bits(j), bits(ii));
    unsigned long ul = -2;
    TraceX(bits(ul), bits(ii));
    bitset<8> b2(2);
    uint64_t ull = (255ull << 56) | (127ull << 48) | (63ull << 40) | (31ull << 32)
        | (15ull << 24) | (7ull << 16) | (3ull << 8) | 1;
    TraceX(bits(ull));
    TraceX(bitset<8>(2).to_string());
    for (size_t k = 0; k != b2.size(); ++k) cout << b2[k];
    cout << endl;
}

int main() {
    arithm_conversion();
    bit_repr();
    return 0;
}