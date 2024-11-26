#pragma once

#include "pdft/tui/alignment.h"
#include "pdft/tui/rect.h"

namespace pdft::tui::image {

struct Placement {
    // The strategy which defines how the image will be displayed inside the rect.
    Alignment hor = Alignment::Center;
    Alignment vert = Alignment::Center;
};

IRect fitImage(float image_wh_cell_ratio, const IRect& rect, const Placement& place);

}  // namespace pdft::tui::image
