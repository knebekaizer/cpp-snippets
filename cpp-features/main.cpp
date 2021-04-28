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

	functional();
    structured();
	spanning();
	scoped();
    outer::nested::nestedNamespaceSample();

//    terminatePolicy();

//    void testPolicy();
//    testPolicy();

    bool x = onErrorDefault(true);
    TraceX(x);
	return 0;
}
