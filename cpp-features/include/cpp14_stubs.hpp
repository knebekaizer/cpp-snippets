// Copyright (c) 2023 Vladimir Ivanov <knebekaizer@gmail.com>.

/**
 * \file
 * C++14 compatibility helpers
 */

#pragma once

#include <type_traits>
#include <utility>
#include <array>

namespace cpp14_stubs {
namespace details {

template<class B>
struct negation : std::integral_constant<bool, !bool(B::value)> { };

template<class...> struct conjunction : std::true_type {};
template<class B1> struct conjunction<B1> : B1 {};
template<class B1, class... Bn>
struct conjunction<B1, Bn...>
    : std::conditional_t<bool(B1::value), conjunction<Bn...>, B1> {};

template<class> struct is_ref_wrapper : std::false_type{};
template<class T> struct is_ref_wrapper<std::reference_wrapper<T>> : std::true_type{};

template<class T>
using not_ref_wrapper = negation<is_ref_wrapper<std::decay_t<T>>>;

template<class D, class...> struct return_type_helper { using type = D; };
template<class... Types>
struct return_type_helper<void, Types...> : std::common_type<Types...>
{
	static_assert(conjunction<not_ref_wrapper<Types>...>::value,
	"Types cannot contain reference_wrappers when D is void");
};

template<class D, class... Types>
using return_type = std::array<typename return_type_helper<D, Types...>::type,
		sizeof...(Types)>;
}

template<class D = void, class... Types>
constexpr details::return_type<D, Types...> make_array(Types&&... t)
{
	return {std::forward<Types>(t)...};
}

namespace details {
template <typename _Tp, typename _Tuple, size_t... _Idx>
constexpr _Tp make_from_tuple_impl(_Tuple&& __t, std::index_sequence<_Idx...>) {
    return _Tp(std::get<_Idx>(std::forward<_Tuple>(__t))...);
}
} // namespace details

template <typename _Tp, typename _Tuple>
constexpr _Tp
    make_from_tuple(_Tuple&& __t)
//        noexcept(__unpack_std_tuple<is_nothrow_constructible, _Tp, _Tuple>)
{
    return details::make_from_tuple_impl<_Tp>(
        std::forward<_Tuple>(__t),
        std::make_index_sequence<std::tuple_size<std::remove_reference_t<_Tuple>>::value>{});
}

template<typename x_Function> class
    function_traits;

// specialization for functions
template<typename x_Result, typename... x_Args> class
    function_traits<x_Result (x_Args...)>
{
public: using arguments = ::std::tuple<x_Args...>;
};
}

using cpp14_stubs::make_array;
using cpp14_stubs::make_from_tuple;
using cpp14_stubs::function_traits;

