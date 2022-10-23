//
// Created by Vladimir on 23.10.2022.
//
#include <algorithm>
#include <ranges>

using namespace std;
namespace rg = std::ranges;
namespace vs = std::ranges::views;

// https://stackoverflow.com/questions/58808030/range-view-to-stdvector


namespace detail {
// Type acts as a tag to find the correct operator| overload
template <typename C>
struct to_helper {
};

// This actually does the work
template <typename Container, rg::range R>
    requires std::convertible_to<rg::range_value_t<R>, typename Container::value_type>
Container operator|(R&& r, to_helper<Container>) {
    return Container{r.begin(), r.end()};
}
}

// Couldn't find an concept for container, however a
// container is a range, but not a view.
template <rg::range Container>
    requires (!rg::view<Container>)
auto to() {
    return detail::to_helper<Container>{};
}

/*
Right, the issue here has nothing to do with views.
 You can sort a view - it's just that sort requires random access, and that gets lost
 with the filter (and also you can't sort a range of prvalues). –


Reversing a random access iterator still gives you a random access iterator, that part is fine.
 transform gives you a prvalue, which makes things non-sortable, but if it gave an lvalue, that'd be fine. e.g.
 sort(iota(0u, v.size()) | transform([&v](int i) -> int& { return v[i]; }));
 is a valid, if ridiculous, way to do sort(v); –
 */

void test_sort() {
    [[maybe_unused]] auto v = vs::iota(0,8);
    sort(vs::iota(0u, v.size()) | vs::transform([&v](int i) -> int& { return v[i]; }));
//    rg::sort(v);
}

int main() {
    return 0;
}