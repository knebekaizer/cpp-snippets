//
// Created by vdi on 20.12.23.
//

#include "trace.hpp"

#include <memory>
using namespace std;

struct Base {
    virtual ~Base() = default;
};

class S : public Base {
    static int n_;
public:
    void operator delete(void* ptr){
        log_trace << "id = " << static_cast<S*>(ptr)->id;
        ::delete static_cast<char*>(ptr);
    }
    ~S() { TraceX(id); }
    int id = ++n_;
};
int S::n_ = 0;

void test_deleter() {
    [[maybe_unused]]
    auto p = make_unique<S>();

    S* x = new S;
    default_delete<S> del;
    del(x);

    default_delete<Base> del2;
    Base* x2 = new S;
    del2(x2);
}

int main() {
    test_deleter();
    return 0;
}
