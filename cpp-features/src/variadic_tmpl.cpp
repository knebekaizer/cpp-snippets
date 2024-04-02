#include <iostream>


template <class ... Ts>
void foo(Ts && ... inputs)
{
	int i = 0;

	([&]
	{
		// Do things in your "loop" lambda

		++i;
		std::cout << "input " << i << " = " << inputs << std::endl;

	} (), ...);
}

template <class ... Ts>
void bar()
{
	int i = 0;

	([&]
	{
		++i;
		std::cout << "type " << i << ": " << typeid(Ts).name() << std::endl;
	} (), ...);
}

int main ()
{
	foo(2, 3, 4u, (int64_t) 9, 'a', 2.3);
	bar<int, int, float>();
}
