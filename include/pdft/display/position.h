#pragma once

#include "pdft/view/rect.h"

namespace pdft::display {

enum class FitStrategy {
    Scale,
    Left,
    Right,
    Center,
};

struct Position {
    view::IRect rect;
    FitStrategy fit = FitStrategy::Center;
};

}  // namespace pdft::display
