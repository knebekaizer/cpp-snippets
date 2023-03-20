
#include <string>
#include <iostream>
/* includes of SequentialTest 'T_iterator_End_Dummy [_ST_YB_1]' */
#include <map>

#include <array>

#include <functional>

class my_class1 {
public:
    int x;
	virtual int get() {
		return x;
	}
bool operator<(my_class1 mk) const {
//less_inside_type_operator_in_user_dfind_type_flag= true;
		return x < mk.x;
	}

bool operator>(my_class1 mk) const {
//greater_inside_type_operator_in_user_dfind_type_flag= true;
		return x > mk.x;
	}
};
int my_class1::*ptr = &my_class1::x;
int my_class1::*ptr1 = &my_class1::x;

template <class T>
class Custom_comparegreater1 {
   public:
             bool operator() (T y, T x) const {
              // my_class1 obj1, obj2;
                 int *x1, *y1;
//              int my_class1::*ptr = x;
             //greater_custom_compare_operator_flag = true;
             
             //x = 1;
	std::cout << sizeof(x) << std::endl;
x1 = reinterpret_cast<int*>(&x);
y1 = reinterpret_cast<int*>(&y);
return *x1>*y1;
                }
             //return &x>&y;   }
};

template <class T>
T get_value(int index);

template<>
int my_class1::* get_value<int my_class1::*>(int index) {
   my_class1 myobj;
   int  my_class1::* p = &my_class1::x;
   myobj.*p=index;
	return  p;
}

template<>
std::array<int, 5> get_value<std::array<int, 5>>(int index) {
    return ( std::array<int, 5>{index, index+1, index+2, index+3, index+4} );
}

enum ec_typ_Sizes {
    ec_Sizes_S0 /* 'EmptyMultimapIteratorEnd [_TV_YB_68]' -> 0' */
   , ec_Sizes_S1 /* 'OneMultimapIteratorEnd [_TV_YB_52]' -> 1' */
   , ec_Sizes_S42 /* 'RandomMultimapIteratorEnd [_TV_YB_19]' -> 42' */
   , ec_Sizes_S1000 /* 'LargeMultimapIteratorEnd [_TV_YB_13]' -> 1000' */
};

size_t Sizes2size_t(ec_typ_Sizes ec) {
   switch (ec) {
      case ec_Sizes_S0: /* TestValue 'EmptyMultimapIteratorEnd [_TV_YB_68]' = 0' */
         return 0;
      case ec_Sizes_S1: /* TestValue 'OneMultimapIteratorEnd [_TV_YB_52]' = 1' */
         return 1;
      case ec_Sizes_S42: /* TestValue 'RandomMultimapIteratorEnd [_TV_YB_19]' = 42' */
         return 42;
      case ec_Sizes_S1000: /* TestValue 'LargeMultimapIteratorEnd [_TV_YB_13]' = 1000' */
         return 1000;
   default: /* unreachable */
      return (size_t)0;
   }
}

template <class T , class U, class C>
std::multimap<T, U, C> make_multimap(ec_typ_Sizes ec);

template <>
std::multimap<int my_class1::*, std::array<int, 5>, Custom_comparegreater1<int my_class1::*>> make_multimap(ec_typ_Sizes ec)
{
   std::multimap<int my_class1::*, std::array<int, 5>, Custom_comparegreater1<int my_class1::*>> m;

   for (unsigned int i = 0; i < Sizes2size_t(ec); i++)
   {
	if(i==4 || i==3 || i==1 || i==2)
	{
      		m.insert(std::pair<int my_class1::*, std::array<int, 5>>(get_value<int my_class1::*>(2), get_value<std::array<int, 5>>(i)));
	}
	else{
      		m.insert(std::pair<int my_class1::*, std::array<int, 5>>(get_value<int my_class1::*>(i), get_value<std::array<int, 5>>(i)));

	}
}
   return m;
}

#include <unordered_set>
int main() {
//	std::unordered_set<int my_class1::*> us;
	static_assert(std::is_trivially_copyable<int my_class1::*>::value, "fghj");
	[[maybe_unused]]
	Custom_comparegreater1<int my_class1::*> cmpgcheck;
/*my_class1 a, b, c;
a.*ptr = -5;
b.*ptr =0;
if (cmpgcheck(ptr,ptr) == false)
{

}*/

	std::multimap<int my_class1::*, std::array<int, 5>, Custom_comparegreater1<int my_class1::*>> input1 = make_multimap<int my_class1::*, std::array<int, 5>, Custom_comparegreater1<int my_class1::*>>(
		ec_Sizes_S42);
}
