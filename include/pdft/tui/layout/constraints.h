#pragma once

#include <variant>

namespace pdft::tui::layout {

namespace constraints {

struct Fraction {
    float fraction;
};

struct Fill {
    uint32_t coeff;
};

struct Length {
    uint32_t length;
};

}  // namespace constraints

using Constraint = std::variant<constraints::Fraction, constraints::Fill, constraints::Length>;

}  // namespace pdft::tui::layout
