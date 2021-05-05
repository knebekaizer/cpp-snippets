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


struct Tmp {  ~Tmp() { TraceF;} };

void byValue(Tmp) { TraceF; }
void byConstRef(Tmp const&) { TraceF; }
void byRvRef(Tmp&&) { TraceF; }

void test_temporary() {
	cout << endl;
	byValue(Tmp{});
	byConstRef(Tmp{});
	byRvRef(Tmp{});
}

int main() {
	log_info << "Start";

	cout << endl;
	functional();

	cout << endl;
	structured();

	cout << endl;
	scoped();
	outer::nested::nestedNamespaceSample();

	cout << endl;
	async();

//	  terminatePolicy();

//	  void testPolicy();
//	  testPolicy();
//	  TraceX(onErrorDefault(true)); // see policy.cpp

	test_temporary();
	return 0;
}
