//
// Created by Vladimir on 10.10.2022.
//

#include <initializer_list>
//#include <algorithm>
//#include <iostream>
//#include <span>

#include "range/v3/all.hpp"

#include "scoped.hpp"

#include "trace.hpp"

using namespace ranges;
namespace rg = ranges;
namespace vs = ranges::views;

using namespace std;
using namespace std::literals;


template <bool longMask = true>
class Bitmap {
public:
    Bitmap() = default;
    Bitmap(std::initializer_list<size_t> ls) { set(ls); }
    void set(size_t k) {
        assert(k < 8 * N);
        buf_[k >> 3] |= (1 << (k & 7));
    }
    void set(std::initializer_list<size_t> ls) {
        for (auto x : ls) set(x);
    }

    bool get(size_t k) const {
        assert(k < 8 * N);
        return buf_[k >> 3] & (1 << (k & 7));
    }
    bool operator[](size_t k) const { return get(k); }
    auto indices() const { return vs::ints(0,longMask ? 48 : 16) | vs::filter( [this](auto k){ return this->get(k);} ); }
    auto indices2() const { return buf_ | vs::for_each([base = -8](auto k) mutable { base += 8;
                   return vs::ints(base, base + 8) | vs::filter( [k](auto bit){ return k & (1 << (bit & 7));}); }); }

    static constexpr size_t N = longMask ? 6 : 2;
    uint8_t buf_[N] = {0};
};

template <bool longMask> auto indices(uint8_t buf[longMask ? 6 : 2]) {
    return make_span(buf, longMask ? 6 : 2) | vs::for_each([base = -8](auto k) mutable {
               base += 8;
               return vs::ints(base, base + 8) | vs::filter([k](auto bit) { return k & (1 << (bit & 7)); });
           });
}

auto indices(uint8_t* buf, bool longMask) {
    return rg::make_span(buf, longMask ? 6 : 2) | vs::for_each([base = -8](auto k) mutable {
               base += 8;
               return vs::ints(base, base + 8) | vs::filter([k](auto bit) { return k & (1 << (bit & 7)); });
           });
}

void bm_test() {
    TraceX(sizeof(Bitmap<1>));
    //    Bitmap bm;
    //    bm.set({0,3,6,8,15,23,37,35,40,46,47});
    Bitmap bm{0,3,6,8,15,23,37,35,40,46,47};
    
    for (auto k = 0; k < bm.N * 8; ++k)
        if (bm[k]) cout << k << ',';
    cout << endl;
    TraceX(bm.indices());
    TraceX(bm.indices2());

    uint8_t buf[2] = {0b10001001, 0b110};
    auto bm2 = (Bitmap<0>*)buf;
    TraceX(bm2->indices2());

    TraceX(indices<0>(buf));
    TraceX(indices<0>(bm.buf_));
    TraceX(indices<1>(bm.buf_));
    TraceX(indices(bm.buf_, false));
    TraceX(indices(bm.buf_, true));
}


int main() {
    auto _ = Scoped {
        [](){ log_trace_(main) << "Starting..."; },
        [](){ log_trace_(main) << "Done."; }
    };

    bm_test();
    return 0;
}
