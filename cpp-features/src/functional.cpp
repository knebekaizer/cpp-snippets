//! @file ### Functional

#include <vector>
#include "trace.h"

using namespace std;

template <typename func>
auto checked(func process) {
	return [=](auto location) { if (location) process(location); };
}

void printAddr(const void* addr) {
	cout << addr << endl;
}


template <typename List, typename func>
void processChildren(List& ls, func process) {
	for (auto& x : ls) {
		process(x);
	}
}

void functional() {
	log_info << "Start " << __func__;

	vector<char const *> list = {"first", nullptr, "third"};
	list.push_back(list[0] + 2);

	processChildren(list, printAddr);

	log_info << "Filtered:";
	processChildren(list, checked(printAddr));

	log_info << "From lambda:";
	processChildren(list, [](auto x) { cout << "0x" << (void *) x << endl; });

	log_info << "Checked lambda:";
	processChildren(list, checked([](auto x) { cout << x << endl; }));
}

