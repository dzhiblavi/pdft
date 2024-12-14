#pragma once

#include "pdft/image/color.h"

#include <span>

namespace pdft::image {

using PaletteRef = std::span<const IRGBColor>;

void applyPalette(std::span<IRGBColor> pixels, PaletteRef palette);

}  // namespace pdft::image
