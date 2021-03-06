//! @file Structured binding - since C++17

#include "../include/trace.h"
#include <unordered_set>

using namespace std;

void structured() {
	static unordered_set<int> set;
	{
		auto[it, inserted] = set.insert(42);
		TraceX(*it, inserted);
	}

	log_info << "Second pass:";
	if (auto [it, inserted] = set.insert(42); inserted) {
		// do whatever
		log_info << *it << " was inserted";
	} else {
		log_info << *it << " is duplicate";
	}
}
