#include <iostream>
#include <stdint.h>
#include "trace.h"
#include "features.h"
#include "policy.h"

using namespace std;

namespace outer::nested {
    void nestedNamespaceSample() {
        TraceF;
    }
}

std::function<bool (bool)> onErrorDefault = [](bool ok) { return ok; };

int main() {
	log_info << "Start";

    cout << endl;
	functional();

    cout << endl;
    structured();

    cout << endl;
	spanning();

    cout << endl;
	scoped();
    outer::nested::nestedNamespaceSample();

    cout << endl;
    async();

//    terminatePolicy();

//    void testPolicy();
//    testPolicy();
//    TraceX(onErrorDefault(true)); // see policy.cpp
	return 0;
}
