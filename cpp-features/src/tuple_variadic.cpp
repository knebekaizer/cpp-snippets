#include "trace.hpp"
#include <iostream>
#include <tuple>

using namespace std;

#if (__cplusplus >= 201700)
template <class ... Ts>
void foo(Ts&& ... inputs)
{
	int i = 0;
	([&] {		// Do things in your "loop" lambda
		++i;
		std::cout << "input " << i << " = " << inputs << std::endl;
	} (), ...);
}

template <class ... Ts>
void bar()
{
	int i = 0;
	([&] {
		++i;
		std::cout << "type " << i << ": " << typeid(Ts).name() << std::endl;
	} (), ...);
}
#else

template <class ... Ts>
void foo(Ts&& ... inputs)
{
	using unused = int[];
	int i = 0;
	(void)unused{ ( [&] {		// Do things in your "loop" lambda
		++i;
		std::cout << "input " << i << " = " << inputs << std::endl;
	} () , ++i)... };
}

template <class ... Ts>
void bar()
{
	using unused = int[];
	int i = 0;
	(void)unused{ ( [&] {
		++i;
		std::cout << "type " << i << ": " << typeid(Ts).name() << std::endl;
	} (), ++i) ...};
}

#endif

template <class ... Ts>
struct VTuple {
	using type = std::tuple<Ts ...>;
};

int main ()
{
	foo(2, 3, 4u, (int64_t) 9, 'a', 2.3);
	bar<int, int, float>();

	[[maybe_unused]]
	VTuple<int, float>::type x;
}
