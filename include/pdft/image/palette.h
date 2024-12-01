#pragma once

#include "pdft/image/color.h"

#include <span>

namespace pdft::image {

using PaletteRef = std::span<const RGBColor>;

void applyPalette(std::span<uint8_t> pixels, PaletteRef palette);

PaletteRef getDefaultPalette();

}  // namespace pdft::image
