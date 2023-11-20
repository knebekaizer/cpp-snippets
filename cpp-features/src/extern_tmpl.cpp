/// @file
#include "extern_tmpl.h"
#include <iostream>

extern template class MyExternTmpl<int>;

int main() {
	MyExternTmpl<int>().foo();
	MyExternTmpl<char>().foo();
	return 0;
}