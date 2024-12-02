#pragma once

#include <variant>

namespace pdft {

using Unit = std::monostate;

static constexpr Unit unit = Unit{};

}  // namespace pdft
