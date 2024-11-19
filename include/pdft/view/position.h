#pragma once

#include "pdft/view/rect.h"

namespace pdft::view {

enum class AxisFit {
    Center,
    Min,
    Max,
};

struct Position {
    // The rectangle which bounds the displayed image.
    // It may have negative coordinates. They will be cropped when displayed.
    view::IRect rect;

    // The strategy which defines how the image will be displayed inside the rect.
    AxisFit hor = AxisFit::Center;
    AxisFit vert = AxisFit::Center;

    bool operator==(const Position&) const = default;
};

}  // namespace pdft::view
