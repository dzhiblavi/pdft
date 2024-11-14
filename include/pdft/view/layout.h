#pragma once

#include "rect.h"

#include <variant>
#include <vector>

namespace pdft::view {

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

class Layout {
 public:
    enum Orientation {
        Horizontal,
        Vertical,
    };

    explicit Layout(Orientation orientation) : orientation_(orientation) {}

    std::vector<IRect> split(IRect rectangle);

    void add(Constraint constraint) {
        constraints_.push_back(constraint);
    }

 private:
    Orientation orientation_;
    std::vector<Constraint> constraints_;
};

template <std::convertible_to<Constraint>... Constraints>
auto vertical(Constraints... constraints) {
    Layout layout(Layout::Orientation::Vertical);
    (layout.add(constraints), ...);
    return layout;
}

template <std::convertible_to<Constraint>... Constraints>
auto horizontal(Constraints... constraints) {
    Layout layout(Layout::Orientation::Horizontal);
    (layout.add(constraints), ...);
    return layout;
}

}  // namespace pdft::view
