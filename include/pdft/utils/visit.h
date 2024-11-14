#pragma once

#include <variant>

namespace pdft::utils {

namespace detail {

template <class... Ts>
struct Overloaded : Ts... {
    using Ts::operator()...;
};

template <class... Ts>
Overloaded(Ts...) -> Overloaded<Ts...>;

}  // namespace detail

template <typename Variant, typename... Visitors>
auto visit(Variant&& variant, Visitors&&... visitors) {
    return std::visit(
        detail::Overloaded{std::forward<Visitors>(visitors)...}, std::forward<Variant>(variant));
}

}  // namespace pdft::utils
