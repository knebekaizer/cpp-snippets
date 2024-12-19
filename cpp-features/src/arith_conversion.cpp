#include "trace.hpp"
#include <iostream>
#include <typeinfo>

#include <bitset>
#include <limits>
#include <climits>
#include <arpa/inet.h>

using namespace std;

#define TYPE(T) #T
template <typename T> struct AType {
//    static constexpr auto id = typeid(T);
    std::string name = typeid(T).name();
    static constexpr auto size = sizeof(T);
    static constexpr auto bits = std::numeric_limits<T>::digits;
    std::ostream& print(std::ostream& os) const {
        return os << name << ':' << size << ':' << bits;
    }
};

template <typename T>
std::ostream& operator<<(std::ostream& os, const AType<T>& t) {
    return t.print(os);
}

void typeIDs() {
    TraceX(AType<char const*>());
    TraceX(AType<char>());
    TraceX(AType<unsigned char>());
    TraceX(AType<signed char>());
    TraceX(AType<int8_t>());
    TraceX(AType<int16_t>());
    TraceX(AType<uint16_t>());
    TraceX(AType<int32_t>());
    TraceX(AType<uint32_t>());
    TraceX(AType<int64_t>());
    TraceX(AType<uint64_t>());
    TraceX(AType<int>()); // i:4:31
    TraceX(AType<unsigned long int>()); // m:8:64
    TraceX(AType<long long>()); // x:8:63
    TraceX(AType<unsigned long long>()); // y:8:64
    TraceX((AType<std::common_type_t<long long, unsigned long>>())); // y:8:64
/*
typeIDs> AType<char const*>() = PKc:8:0
typeIDs> AType<char>() = c:1:7
typeIDs> AType<unsigned char>() = h:1:8
typeIDs> AType<signed char>() = a:1:7
typeIDs> AType<int8_t>() = a:1:7
typeIDs> AType<int16_t>() = s:2:15
typeIDs> AType<uint16_t>() = t:2:16
typeIDs> AType<int32_t>() = i:4:31
typeIDs> AType<uint32_t>() = j:4:32
typeIDs> AType<int64_t>() = l:8:63
typeIDs> AType<uint64_t>() = m:8:64
typeIDs> AType<int>() = i:4:31
typeIDs> AType<unsigned long int>() = m:8:64
typeIDs> AType<long long>() = x:8:63
typeIDs> AType<unsigned long long>() = y:8:64
typeIDs> (AType<std::common_type_t<long long, unsigned long>>()) = y:8:64
*/
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