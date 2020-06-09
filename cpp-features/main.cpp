#include <iostream>
#include <stdint.h>
#include "trace.h"
#include "features.h"
#include "policy.h"

using namespace std;

void test2();

int main() {
	log_info << "Start";
//	test2();

	functional();
	structural();
	spanning();
	scoped();
	policy();

	return 0;
}
