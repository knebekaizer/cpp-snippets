//
// Created by vdi on 26.04.24.
//
#include <array>
#include <set>
#include <vector>
#include <bitset>
#include <iterator>
#include <algorithm>

#include "trace.hpp"
#include "range_print.hpp"

using namespace std;


void sorted_input() {
    array a{1,2,3,4,5,6,7,8};
    array b{2,4,6,8};
    vector<decltype(a)::value_type> o;
    set_intersection(a.cbegin(), a.cend(), b.cbegin(), b.cend(), back_inserter(o));
    TraceX(a);
    TraceX(b);
    TraceX(o);
}

void unsorted_input() {
    array a{1,2,3,4,5,6,7,8};
    array b{2,4,12,6,8};
    vector<decltype(a)::value_type> o;
    set_intersection(a.cbegin(), a.cend(), b.cbegin(), b.cend(), back_inserter(o));
    TraceX(b);
    TraceX(o);
}

//! comp function below is the semiordering function, not weak ordering
void semiorder_compare() {
    array a{1, 2, 3, 4, 5, 6, 7, 8};
    array b{2, 6, 4, 8, 6, 8};
    vector<decltype(a)::value_type> o;
    auto comp = [](auto a, auto b) { return a + 2 < b; };
    set_intersection(a.cbegin(), a.cend(), b.cbegin(), b.cend(), back_inserter(o), comp);
    TraceX(b);
    TraceX(o);
}

//! comp function below is the valid weak ordering function
void weakorder_compare() {
    array a{1,2,3,4,5,6,7,8};
    array b{2,6,4,8,6,8};
    vector<decltype(a)::value_type> o;
    auto round2 = [](auto a){ return (a + 1) & static_cast<decltype(a)>(~1ull); };
    auto comp = [round2](auto a, auto b){ return round2(a) < round2(b); };
    set_intersection(a.cbegin(), a.cend(), b.cbegin(), b.cend(), back_inserter(o), comp);
    TraceX(b);
    TraceX(o);
}

//! Output to the set wich has another sort ordering
void out_sorted_other() {
    array a{1,2,3,4,5,6,7,8};
    array b{2,6,4,8,6,8};
    auto more = [](auto a, auto b){ return b < a; };
    set<decltype(a)::value_type, decltype(more)> o;
    auto round2 = [](auto a){ return (a + 1) & static_cast<decltype(a)>(~1ull); };
    auto comp = [round2](auto a, auto b){ return round2(a) < round2(b); };
    set_intersection(a.cbegin(), a.cend(), b.cbegin(), b.cend(), inserter(o), comp);
    TraceX(b);
    TraceX(o);
}



int main() {
    sorted_input();
    unsorted_input();
    semiorder_compare();
    weakorder_compare();
    return 0;
}