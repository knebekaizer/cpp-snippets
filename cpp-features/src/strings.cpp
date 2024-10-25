#include "trace.hpp"

#include <string>
using namespace std;

/*
 * 21.4.6.3
20 Effects: Equivalent to assign(basic_string(first, last)).
21 Returns: *this.

According with this statement the expected effect is:
    (a) creates temporary string object  `basic_string(first, last)`
    using the signature and requirements 21.4.2-13, 21.4.2-14:
`template<class InputIterator> basic_string(InputIterator begin, InputIterator end, const Allocator& a = Allocator())`

                 and then
    (b) calls move assignment function 21.4.6.3-3
`basic_string& assign(basic_string&& str) noexcept`

    The problem is related to the allocator propagation rules. Allocator propagation is configured with
    allocator_traits<allocator_type>::propagate_on_container_swap::value



    I need to check why this specialization memory:966 is ever used. The more common specialization is memory:988

    Actually looks like a bug in the customer code. The function memory:966 is called only once from

    dw::core::SafePtrTest::testInterface <SafePtrTest.cpp:32>

        and never used in the production code.



    memory:966 is a "exotic" specialization of pointer_to

    The "normal" specialization is memory:988. This function is used heavily in the product code, but not from
 */

/**
 * allocator_traits<allocator_type>::propagate_on_container_swap::value
 */
template <class Alloc>
void check_allocator() {
    TraceX(allocator_traits<string::allocator_type>::propagate_on_container_swap::value);
    TraceX(allocator_traits<string::allocator_type>::propagate_on_container_move_assignment::value);
    TraceX(allocator_traits<string::allocator_type>::propagate_on_container_copy_assignment::value);
}

void test_1() {
    string a{"123456"};
    string b{"abcdef"};
    a.assign("qwerty"s);
    using AllocatorType = decltype(a.get_allocator());
    static_assert(is_same_v<AllocatorType, string::allocator_type>);
    check_allocator
    TraceX(allocator_traits<string::allocator_type>::propagate_on_container_swap::value);
    TraceX(allocator_traits<string::allocator_type>::propagate_on_container_move_assignment::value);
    TraceX(allocator_traits<string::allocator_type>::propagate_on_container_copy_assignment::value);
}

int main() {
    cout << boolalpha;
    test_1();
    return 0;
}