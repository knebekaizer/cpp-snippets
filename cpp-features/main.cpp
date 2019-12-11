#include <iostream>
#include <stdint.h>
#include "trace.h"
#include "features.h"

using namespace std;

int main() {
	log_info << "Start";

	functional();
	structural();
	spanning();

	return 0;
}
