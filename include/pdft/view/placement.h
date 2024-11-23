#pragma once

#include "pdft/view/axis_fit.h"

namespace pdft::view {

struct ImageProperties {
    float scaling_factor = 1.f;
    bool invert_colors = false;

    bool operator==(const ImageProperties&) const = default;
};

struct PagePlacement {
    enum class FitMode {
        FitInside,
        CropAndFill,
    };

    // The strategy which defines how the image will be displayed inside the rect.
    AxisFit hor = AxisFit::Center;
    AxisFit vert = AxisFit::Center;

    // How to display the image inside the rect.
    FitMode fit = FitMode::FitInside;
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
    OverflowMode overflow = OverflowMode::Wrap;
};

}  // namespace pdft::view
