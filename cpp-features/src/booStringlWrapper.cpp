//
// Created by vdi on 20.03.24.
//
#include "trace.hpp"
#include <string>

using namespace std;
namespace boolWrapper {
string g_s = "abc";
int ln = 0;
bool isSameBuf(string const& s) { return (s.data() == g_s.data()); }
void foo(auto x, string const& s) { log_trace << x << ", " << (s==g_s) << ", " << isSameBuf(s); }
void bar(auto x, string&& s)  { log_trace << x << ", " << (s==g_s) << ", " << isSameBuf(s); }
}


// Old stuff. Probably an attempt to have bool conversion for a string
void boolStringWrapper() {
    using namespace boolWrapper;
    struct XString {
        XString(std::string& s) : s_(s) {}
        operator std::string const&() const& { return s_; }   // NOLINT(google-explicit-constructor)
        operator std::string&() & { return s_; }              // NOLINT(google-explicit-constructor)
        operator std::string&&() && { return std::move(s_); } // NOLINT(google-explicit-constructor)
        explicit operator bool() const { return !s_.empty(); }
        std::string& s_;
    };

    auto check = [](auto const& s){ return s ? "not empty" : "empty"; };
    XString xs = g_s;
    foo(1, xs.s_);

    string ss = xs; // copy, as expected
    foo(2, xs);

    auto xss = xs;
    foo(3, xss);    // no copy; expected copy

    foo(4, ss);
    //	bar(xs);    // won't compile - as expected
    TraceX(check(xs), g_s);
    bar(5, std::move(xs)); // NOLINT(performance-move-const-arg)
    TraceX(check(xs), g_s);
    //	TraceX(check(ss), g_s);  // OK: no viable conversion from 'const std::string' to 'bool'
    g_s.resize(0);
    TraceX(check(xs), g_s);

    struct String : public std::string {
        String() = delete;
        explicit operator bool() const { return !empty(); }
    };
    g_s = "cba";
    String& a = static_cast<String&>(g_s);
    foo(11, a);
    auto b = a;
    foo(12, b);

    TraceX(check(a), g_s);
    bar(13, std::move(a)); // no move, reference copied
    TraceX(check(a), g_s);
    bar(14, std::move(std::remove_reference<decltype(a)>::type(a))); // bar> 14, true, false  WTF? it's copy
    TraceX(check(a), g_s);
}

int main() {
    boolStringWrapper();
    return 0;
}