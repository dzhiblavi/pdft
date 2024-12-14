#pragma once

#include "pdft/utils/overloaded.h"

#include <variant>

namespace pdft::utils {

template <typename Variant, typename... Visitors>
auto visit(Variant&& variant, Visitors&&... visitors) {
    return std::visit(
        Overloaded{std::forward<Visitors>(visitors)...}, std::forward<Variant>(variant));
}

}  // namespace pdft::utils
