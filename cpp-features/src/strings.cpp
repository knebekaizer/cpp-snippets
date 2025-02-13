#include "trace.hpp"

#include <string>
using namespace std;

// See StackOverflow replies to this answer for important commentary about inheriting from std::allocator before replicating this code.

/// minimal example
namespace min {
template <class T> class allocator {
public:
    using value_type = T;

    //     using pointer       = value_type*;
    //     using const_pointer = typename std::pointer_traits<pointer>::template
    //                                                     rebind<value_type const>;
    //     using void_pointer       = typename std::pointer_traits<pointer>::template
    //                                                           rebind<void>;
    //     using const_void_pointer = typename std::pointer_traits<pointer>::template
    //                                                           rebind<const void>;

    //     using difference_type = typename std::pointer_traits<pointer>::difference_type;
    //     using size_type       = std::make_unsigned_t<difference_type>;

    //     template <class U> struct rebind {typedef allocator<U> other;};

    allocator() noexcept {} // not required, unless used
    template <class U> allocator(allocator<U> const&) noexcept {}

    value_type* // Use pointer if pointer is not a value_type*
        allocate(std::size_t n) {
        return static_cast<value_type*>(::operator new(n * sizeof(value_type)));
    }

    // Use pointer if pointer is not a value_type*
    void deallocate(value_type* p, std::size_t) noexcept  {
        ::operator delete(p);
    }
};

template <class T, class U> bool operator==(allocator<T> const&, allocator<U> const&) noexcept {
    return true;
}

template <class T, class U> bool operator!=(allocator<T> const& x, allocator<U> const& y) noexcept {
    return !(x == y);
}
using string = std::basic_string<char, std::char_traits<char>, allocator<char>>;
} // namespace min

namespace my {
template <class T> class allocator {
public:
    using value_type = T;
    static int g_sn;
    int sn = -1;

    allocator() noexcept : sn(++g_sn) { /*log_trace << "default ctor: " << sn;*/ } // not required, unless used
    allocator(allocator const& a) noexcept : sn(a.sn) { /*log_trace << "copy ctor: " << sn;*/ }
    allocator(allocator&& a) noexcept : sn(a.sn) { /*log_trace << "move ctor: " << sn;*/ }
    allocator& operator=(allocator const& a) noexcept {  sn = a.sn; /*log_trace << "copy assign: " << sn;*/ return *this; }
    allocator& operator=(allocator&& a) noexcept { sn = a.sn; /*log_trace << "move assign: " << sn;*/ return *this; }
    template <class U> allocator(allocator<U> const&) noexcept { /*log_trace << "copy<U> ctor: " << sn;*/ }
    template <class U> allocator(allocator<U>&&) noexcept { /*log_trace << "move<U> ctor: " << sn;*/ }
    template <class U> allocator& operator=(allocator<U> const&) noexcept { /*log_trace << "copy<U> assign: " << sn;*/ return *this; }
    template <class U> allocator& operator=(allocator<U>&&) noexcept { /*log_trace << "move<U> assign: " << sn;*/ return *this; }

    // Use pointer if pointer is not a value_type*
    value_type* allocate(std::size_t n) {
        return static_cast<value_type*>(::operator new(n * sizeof(value_type)));
    }

    // Use pointer if pointer is not a value_type*
    void deallocate(value_type* p, std::size_t) noexcept  {
        ::operator delete(p);
    }

    using propagate_on_container_move_assignment = std::true_type;
};

template <class T, class U> bool operator==(allocator<T> const&, allocator<U> const&) noexcept {
    return true;
}

template <class T, class U> bool operator!=(allocator<T> const& x, allocator<U> const& y) noexcept {
    return !(x == y);
}
using string = std::basic_string<char, std::char_traits<char>, allocator<char>>;

template<> int allocator<char>::g_sn = 0;
} // namespace my

/*
 * 21.4.6.3
20 Effects: Equivalent to assign(basic_string(first, last)).
21 Returns: *this.

According to this statement the expected effect is:
    (a) creates temporary string object  `basic_string(first, last)`
    using the signature and requirements 21.4.2-13, 21.4.2-14:
`template<class InputIterator> basic_string(InputIterator begin, InputIterator end, const Allocator& a = Allocator())`

                 and then
    (b) calls move assignment function 21.4.6.3-3
`basic_string& assign(basic_string&& str) noexcept`

    The problem is related to the allocator propagation rules. Allocator propagation is configured with
    allocator_traits<allocator_type>::propagate_on_container_swap::value
 */

/**
 * allocator_traits<allocator_type>::propagate_on_container_swap::value
 */
template <class Alloc>
void check_allocator() {
    TraceX(allocator_traits<Alloc>::propagate_on_container_swap::value);
    TraceX(allocator_traits<Alloc>::propagate_on_container_move_assignment::value);
    TraceX(allocator_traits<Alloc>::propagate_on_container_copy_assignment::value);
}

void test_alloc() {
    my::string a{"123456"};
    const char b[] = "abcdef";
    const size_t n = sizeof(b) - 1;
    a.assign("qwerty"s);
    a.assign(b, n);
    a.assign(b, b + n);
    a.assign({65, 66, 67, 68});
    a.assign(3,65);

    auto a1 = a.get_allocator();
    TraceX(1, &a1, a1.sn);
    auto a2 = a.get_allocator();
    TraceX(2, &a2, a2.sn);
    my::string x{"123456"};
    TraceX(3, a.get_allocator().sn);
    a.assign(x);
    TraceX(4, a.get_allocator().sn);
    a.assign(std::move(x));
    TraceX(5, a.get_allocator().sn);
    a.assign(my::string(b, b + n));
    TraceX(6, a.get_allocator().sn);
}

void test_buffer() {
    string s("1234567890123456789 Secret");
    string a("9876543210987654321");
    auto secret = [&](){ return string_view(s.data() + a.size() + 1, 6);};
    log_trace << "1: " << secret();
    s.assign(a.begin(), a.end());
    TraceX(2, (void*)s.data());
    log_trace << "2: " << secret();
    s.assign(string(a.begin(), a.end()));
    TraceX(3, (void*)s.data());
    log_trace << "3: " << secret();

    s.swap(a);
    TraceX(4, (void*)a.data());
    TraceX(4, (void*)s.data());
    log_trace << "4: " << secret();
}

void test_buffer_access() {
    string secretStr("1234567890123456789 Secret");
    const size_t secretOffset = secretStr.find("Secret"); // 20
    TraceX(secretOffset);
    const size_t initialSize = secretStr.size();
    string newData("9876543210987654321");

    // the lambda attempts to access the tail of the buffer
    auto print_secret = [&](auto&& s) {
        // if capacity is sufficient then try to access and print the buffer contents
        if (s.capacity() >= initialSize)
            std::cout << string_view(s.data() + newData.size() + 1, 6) << std::endl;
        else
            std::cout << "Not accessible\n";
    };

    std::cout << "Original data: ";
    print_secret(secretStr);

    secretStr.assign(newData.begin(), newData.end());
    std::cout << "New data after assigning the range: ";
    print_secret(secretStr);

    secretStr.assign(string(newData.begin(), newData.end()));
    std::cout << "New data as it's supposed to be: ";
    print_secret(secretStr);
}

/**
 * SSO = Short String Optimization
 * no swap() function invalidates any references, pointers, or iterators referring to the elements of the
 * containers being swapped. [ Note: The end() iterator does not refer to any element, so it may be
 * invalidated. —end note ]
 */
void test_SSO() {
	{
		string s1 = "123";
		auto p = &s1[1];
		string s2(std::move(s1));
		TraceX((p == &s2[1]));
	}
	{
		string s1(100, 'a');
		auto p = &s1[1];
		string s2(std::move(s1));
		TraceX((p == &s2[1]));
	}
	{
		string s1 = "123";
		auto p = &s1[1];
		TraceX(1, *p);
		string s2("456");
		s1.swap(s2);
		TraceX(2, *p);
	}
	{
		string s1 = "123";
		auto p = &s1[1];
		TraceX(1, *p);
		string s2(100, 'a');
		s1.swap(s2);
		TraceX(2, *p);
	}
}

int main() {
    cout << boolalpha;
    test_alloc();
    test_buffer();
    test_buffer_access();
	check_allocator<std::allocator<char>>();
	test_SSO();
    return 0;
}