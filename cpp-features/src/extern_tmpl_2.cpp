/// @file
#include "extern_tmpl.h"
#include <iostream>

template <>
void MyExternTmpl<int>::foo() {
	std::cout << "Specialization: " <<  __PRETTY_FUNCTION__ << std::endl;
}
