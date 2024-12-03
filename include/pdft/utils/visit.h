#pragma once

#include "pdft/utils/instanceof_template.h"
#include "pdft/utils/overloaded.h"

#include <tuple>
#include <variant>

namespace pdft::utils {

template <typename Variant, typename... Visitors>
auto visit(Variant&& variant, Visitors&&... visitors) {
    return std::visit(
        Overloaded{std::forward<Visitors>(visitors)...}, std::forward<Variant>(variant));
}

namespace detail {

template <typename V>
struct UnpackingVisitor {
    V visitor;

    template <typename T>
    requires(IsInstanceOfTemplate<std::decay_t<T>, std::tuple>)
    decltype(auto) operator()(T&& tuple) {
        return std::apply(visitor, std::forward<T>(tuple));
    }

    template <typename T>
    requires(!IsInstanceOfTemplate<std::decay_t<T>, std::tuple>)
    decltype(auto) operator()(T&& value) {
        return visitor(std::forward<T>(value));
    }
};

}  // namespace detail

// Unpacks tuples once before passing them to visitors
template <typename Variant, typename... Visitors>
auto visitUnpack(Variant&& variant, Visitors&&... visitors) {
    return std::visit(
        detail::UnpackingVisitor{Overloaded{std::forward<Visitors>(visitors)...}},
        std::forward<Variant>(variant));
}

}  // namespace pdft::utils
