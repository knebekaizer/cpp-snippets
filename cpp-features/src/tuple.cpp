#include <tuple>
#include <iostream>

template<std::size_t I = 0, typename FuncT, typename... Tp>
inline typename std::enable_if<I == sizeof...(Tp), void>::type
    for_tuple(std::tuple<Tp...> &, FuncT) // Unused arguments are given no names.
{ }

template<std::size_t I = 0, typename FuncT, typename... Tp>
    inline typename std::enable_if<I < sizeof...(Tp), void>::type
    for_tuple(std::tuple<Tp...>& t, FuncT f)
{
    f(std::get<I>(t));
    for_tuple<I + 1, FuncT, Tp...>(t, f);
}

void B::accept(Json& j) const {
    auto getValue [[maybe_unused]] = [this](auto f) {
        static_assert(std::is_member_pointer<decltype(f)>::value, "Illegal type");
        if constexpr (std::is_member_object_pointer<decltype(f)>::value)
            return this->*f;
        else if constexpr (std::is_member_function_pointer<decltype(f)>::value)
            return (this->*f)();
    };
    auto render = [&j, getValue, line=0](auto& pair) mutable {
        if (line++) j << ",\n";
        j.line() << quoted(pair.first) << ": " << getValue(pair.second);
    };
    static auto dict = getSerializer(); // tuple
    for_tuple(dict, render);
}


template<class... Ts> void printAll(Ts&&... args)
{
    //	f(&args...); // “&args...” is a pack expansion
    // “&args” is its pattern
    (std::cout << ... << args) << '\n';
}
