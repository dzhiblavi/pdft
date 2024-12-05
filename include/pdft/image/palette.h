#pragma once

#include "pdft/image/color.h"

#include <span>

namespace pdft::image {

using PaletteRef = std::span<const RGBColor>;

void applyPalette(std::span<RGBColor> pixels, PaletteRef palette);

}  // namespace pdft::image
