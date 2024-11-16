#pragma once

#include "pdft/view/rect.h"

namespace pdft::display {

enum class FitStrategy {
    Scale,   // Scale the image to fit the bounds.
    Center,  // Display thet whole image preserving its ratio. Center it inside the bounds.
    Left,   // Display the whole image preserving its ratio. Glue it to the left side of the bounds.
    Right,  // Display the whole image preserving its ratio. Glue it to the right side of the
            // bounds.
    ClipLeft,   // Fill the bounds with the left/top part of the image, preserving its ratio.
    ClipRight,  // Fill the bounds with the right/bottom part of the image, preserving its ratio.
};

struct Position {
    // The rectangle which bounds the displayed image.
    view::IRect rect;

    // The strategy which defines how the image will be displayed inside the rect.
    FitStrategy fit = FitStrategy::Center;
};

}  // namespace pdft::display
