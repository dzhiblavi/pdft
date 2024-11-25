#pragma once

#include "pdft/tui/layout/alignment.h"
#include "pdft/tui/layout/rect.h"

#include <tuple>

namespace pdft::tui::image {

struct Placement {
    enum class FitMode {
        FitInside,
        CropAndFill,
    };

    // The strategy which defines how the image will be displayed inside the rect.
    layout::Alignment hor = layout::Alignment::Center;
    layout::Alignment vert = layout::Alignment::Center;

    // How to display the image inside the rect.
    FitMode fit = FitMode::FitInside;
};

std::tuple<layout::IRect, layout::FRect> fitImage(
    float wh_ratio, const layout::IRect& rect, const Placement& place);

}  // namespace pdft::tui::image
