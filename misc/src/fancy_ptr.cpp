/*******************************************************************************
 * Copyright (c) 2022
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * SPDX-License-Identifier: Apache-2.
 *******************************************************************************/

/*!
 * @file
 * An example of dependent types at template instantiation.
 *
 * my::custom_allocator is essentially a copy of std::allocator
 * The only difference is the allocater::pointer type which is declared as class Ptr
 * instead of builtin pointer type.
 * 
 * Consider the call graph generated for foo_adapter_custom vs. foo_adapter_default
 */

#include "fancy_ptr.h"

#include <iostream>
#include <cassert>
#include <vector>

using namespace std;

extern "C" int foo([[maybe_unused]] const int* buf, [[maybe_unused]] unsigned long n) {
	return 0;
}

//! Imitate C++ wrapper for plain C API
int foo_adapter_default(const std::vector<int>& v) {
    return foo(&v[0], v.size());
}

typedef std::vector<int, my::custom_allocator<int>> FancyVec;

//! Imitate C++ wrapper for plain C API
int foo_adapter_custom(const FancyVec& v) {
    return foo(&v[0], v.size());
}

int foo_at_rvalue(const FancyVec& v, size_t n) {
	return v.at(n);
}

void foo_at_lvalue(FancyVec& v, size_t n) {
	v.at(n) = 42;
}

void test() try {
	FancyVec v(10);
	v.insert(v.end(), 42);
	assert(v.size() == 11);
} catch (std::exception& e) {
	cerr << "[ERROR] terminating with exception:\n" << e.what() << endl;
//	throw;
}

// Explicit instantiation
template class std::vector<int, my::custom_allocator<int>>;

int main() {
	test();
	return 0;
}