/// @file
#include <iostream>

template <typename T>
class MyExternTmpl {
public:
	inline void foo();
};

template <typename T>
void MyExternTmpl<T>::foo() {
	std::cout << __PRETTY_FUNCTION__ << std::endl;
}

extern template class __attribute__((__visibility__("default"))) MyExternTmpl<int>;

int main() {
	MyExternTmpl<int>().foo();
	MyExternTmpl<char>().foo();
	return 0;
}