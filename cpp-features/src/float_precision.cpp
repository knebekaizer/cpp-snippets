//
// Created by vdi on 28.03.24.
//

#include <bitset>
#include <cassert>
#include <climits>
#include <cmath>
#include <iostream>
#include <limits>
#include <random>
#include <typeinfo>
#include "trace.hpp"
using namespace std;

void testDouble() {
    using F = long double;
    F a = 0;
    F b = 42;
    F x = nextafter(b, a);
    cout << boolalpha;
    TraceX(x < b);
    TraceX((int32_t)(x - a));
}

void testF() {
    using Int = int32_t;
    const Int N = 100000;
    for (Int i = 0; i < N; ++i) {
        auto x = (double)i;
        if ((Int)x != i) TraceX(i, x);
    }
}

template<class G>
typename std::enable_if<std::is_same<G, std::ranlux24>::value ||
                            std::is_same<G, std::ranlux48>::value ||
                            std::is_same<G, std::knuth_b>::value,
                        G>::type
    create_generator() {
    TraceX(1, typeid(G).name());
    std::mt19937 gen(42);
    return G(gen());
}
template<class G>
typename std::enable_if<!std::is_same<G, std::ranlux24>::value &&
                            !std::is_same<G, std::ranlux48>::value &&
                            !std::is_same<G, std::knuth_b>::value,
                        G>::type
    create_generator() {
    TraceX(2, typeid(G).name());
    return G(std::random_device{}());
}

template <typename RE>
void test_random() {
    using F = float;
    std::uniform_real_distribution<F> distribution(1.0, 2.0);
    std::uniform_real_distribution<F>::param_type param(0, 42);
//    auto g = RE(random_device{}());
    auto g = create_generator<RE>();
    using Int = size_t;
    const Int N = 100000;
    for (int k = 0; k < 100; ++k)
    for (Int i = 0; i < N; ++i) {
        auto x = distribution(g, param);
        if (x >= param.b()) TraceX("FAILED", x, param.b());
        int n = static_cast<int>(x - param.a());
        if (n >= 42) TraceX(n, x);
        assert(n < 42);
    }
}

void testAllRandom() {
//    test_random<std::default_random_engine>();
//    test_random<std::knuth_b>();
    test_random<std::minstd_rand0>();
    test_random<std::minstd_rand>();
//    test_random<std::mt19937>();
//    test_random<std::mt19937_64>();
//    test_random<std::ranlux24>();
//    test_random<std::ranlux24_base>();
//    test_random<std::ranlux48>();
    test_random<std::ranlux48_base>();
}

//string floatToBin2(float f) {
//    constexpr unsigned int Size = sizeof(f) * CHAR_BIT;
//    bitset<Size> bits{*reinterpret_cast<unsigned int*>(&f)};
//    return bits.to_string();
//}

string floatToBin(float f) {
    auto* buf = (const unsigned char*)&f;
    string s;
    for (auto k = sizeof(f); k-->0; ++buf)
        s += bitset<8>{*buf}.to_string();
    return s;
}

void typesRepresentation() {
    TraceX(typeid(short).name(), typeid(unsigned short).name(), sizeof(short));
    TraceX(typeid(int).name(), typeid(unsigned int).name(), sizeof(int));
    TraceX(typeid(long).name(), typeid(unsigned long).name(), sizeof(long));
    TraceX(typeid(long long).name(), typeid(unsigned long long).name(), sizeof(long long));
    TraceX(floatToBin(.0f));
    TraceX(floatToBin(1.0f));
    TraceX(floatToBin(2.0f));
//    TraceX(floatToBin2(2.0f));
    TraceX(floatToBin(4.0f));
    TraceX(floatToBin(42.0f));
//    TraceX(floatToBin2(42.0f));
    TraceX(floatToBin(43.0f));
    TraceX(numeric_limits<float>::digits);
}


#include <iostream>
#include <complex>
#include <cmath>
#include <limits>

using T = long double;

template <class _Tp>
std::complex<_Tp> test_division_operator(const std::complex<_Tp>& __z, const std::complex<_Tp>& __w) {
  _Tp a = __z.real();
  _Tp b = __z.imag();
  _Tp c = __w.real();
  _Tp d = __w.imag();

  std::cout << "=== Traditional Division Test ===" << std::endl;
  std::cout << "Input: (" << a << " + " << b << "i) / (" << c << " + " << d << "i)" << std::endl;

  _Tp den, r, re, im;

  if (fabs(c) < fabs(d))
  {
    std::cout << "Using fabs(c) < fabs(d) branch" << std::endl;
    r = c / d;
    std::cout << "r = c / d = " << c << " / " << d << " = " << r << std::endl;
    den = (c * r) + d;
    std::cout << "den = (c * r) + d = (" << c << " * " << r << ") + " << d << " = " << den << std::endl;

    _Tp numerator_re = (a * r) + b;
    _Tp numerator_im = (b * r) - a;
    std::cout << "numerator_re = (a * r) + b = (" << a << " * " << r << ") + " << b << " = " << numerator_re << std::endl;
    std::cout << "numerator_im = (b * r) - a = (" << b << " * " << r << ") - " << a << " = " << numerator_im << std::endl;

    re = numerator_re / den;
    im = numerator_im / den;
  }
  else
  {
    std::cout << "Using fabs(c) >= fabs(d) branch" << std::endl;
    r = d / c;
    std::cout << "r = d / c = " << d << " / " << c << " = " << r << std::endl;
    den = (d * r) + c;
    std::cout << "den = (d * r) + c = (" << d << " * " << r << ") + " << c << " = " << den << std::endl;

    _Tp numerator_re = a + (b * r);
    _Tp numerator_im = b - (a * r);
    std::cout << "numerator_re = a + (b * r) = " << a << " + (" << b << " * " << r << ") = " << numerator_re << std::endl;
    std::cout << "numerator_im = b - (a * r) = " << b << " - (" << a << " * " << r << ") = " << numerator_im << std::endl;

    re = numerator_re / den;
    im = numerator_im / den;
  }

  std::cout << "Initial result: re = " << re << ", im = " << im << std::endl;

  if (std::isnan(re) && std::isnan(im))
  {
    std::cout << "Both re and im are NaN, checking special cases..." << std::endl;
    if (den == 0.0 && (!std::isnan(a) || !std::isnan(b)))
    {
      std::cout << "Special case: denominator is zero with finite numerator" << std::endl;
      re = std::copysign(INFINITY, c) * a;
      im = std::copysign(INFINITY, c) * b;
      std::cout << "After special case 1: re = " << re << ", im = " << im << std::endl;
    }
    else if ((std::isinf(a) || std::isinf(b)) && std::isfinite(c) && std::isfinite(d))
    {
      std::cout << "Special case: infinite numerator with finite denominator" << std::endl;
      a = std::copysign(std::isinf(a) ? 1 : 0, a);
      b = std::copysign(std::isinf(b) ? 1 : 0, b);
      re = INFINITY * (a * c + b * d);
      im = INFINITY * (b * c - a * d);
      std::cout << "After special case 2: re = " << re << ", im = " << im << std::endl;
    }
    else if ((std::isinf(c) || std::isinf(d)) && std::isfinite(a) && std::isfinite(b))
    {
      std::cout << "Special case: infinite denominator with finite numerator" << std::endl;
      c = std::copysign(std::isinf(c) ? 1 : 0, c);
      d = std::copysign(std::isinf(d) ? 1 : 0, d);
      re = 0.0 * (a * c + b * d);
      im = 0.0 * (b * c - a * d);
      std::cout << "After special case 3: re = " << re << ", im = " << im << std::endl;
    }
    else
    {
      std::cout << "No special case matched, keeping NaN values" << std::endl;
    }
  }
  else
  {
    std::cout << "No NaN handling needed" << std::endl;
  }

  std::cout << "Final result: (" << re << " + " << im << "i)" << std::endl;
  std::cout << "==================================" << std::endl;

  return std::complex<_Tp>(re, im);
}

int test_complex() {
    std::cout << "Testing Complex Division Operator" << std::endl;
    std::cout << "=================================" << std::endl;

    // Test case 1: Normal values
    std::complex<T> c1(3.0L, 4.0L);
    std::complex<T> c2(1.0L, 2.0L);

    std::cout << "\nTest 1: Normal values" << std::endl;
    auto result1_custom = test_division_operator(c1, c2);
    auto result1_std = c1 / c2;
    std::cout << "Custom result: " << result1_custom << std::endl;
    std::cout << "Std result:    " << result1_std << std::endl;

    // Test case 2: Extreme values (like in your test)
    std::complex<T> c3(-42.0L, -1.18973e+4932L);
    std::complex<T> c4(-1.18973e+4932L, -42.0L);

    std::cout << "\nTest 2: Extreme values" << std::endl;
    auto result2_custom = test_division_operator(c3, c4);
    auto result2_std = c3 / c4;
    std::cout << "Custom result: " << result2_custom << std::endl;
    std::cout << "Std result:    " << result2_std << std::endl;

    return 0;
}

int main() {
	test_complex();
	return 0;
//    typesRepresentation();
    testDouble();
    testF();
    for (int i = 0; i < 20; ++i) {
        testAllRandom();
    }
    return 0;
}

