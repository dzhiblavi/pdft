#pragma once

#include "pdft/tui/layout/constraints.h"
#include "pdft/tui/layout/rect.h"

#include <vector>

namespace pdft::tui::layout {

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

}  // namespace pdft::tui::layout
