/// @file
#include <iostream>

template <typename T>
class MyExternTmpl {
public:
	inline void foo() {
		std::cout << "Generic form: " << __PRETTY_FUNCTION__ << std::endl;
	}

};

//template class __attribute__((__visibility__("default"))) MyExternTmpl<int>;
//template class MyExternTmpl<int>;
