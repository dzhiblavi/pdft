#pragma once

#include "pdft/view/axis_fit.h"

namespace pdft::view {

struct PagePlacement {
    // The strategy which defines how the image will be displayed inside the rect.
    AxisFit hor = AxisFit::Center;
    AxisFit vert = AxisFit::Center;
};

struct TextPlacement {
    enum class OverflowMode {
        Truncate,  // Discard excessive text
        Wrap,      // Wrap excessive text to the next line(s)
    };

    // The strategy which defines how the text will be displayed inside the rect.
    AxisFit hor = AxisFit::Center;
    AxisFit vert = AxisFit::Center;

    // What to do with text that overflows
    OverflowMode overflow;
};

}  // namespace pdft::view
