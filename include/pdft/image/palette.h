#pragma once

#include "pdft/image/color.h"

#include <span>
#include <unordered_map>

namespace pdft::image {

using PaletteRef = std::span<const RGBColor>;

void applyPalette(std::span<RGBColor> pixels, PaletteRef palette);

const std::unordered_map<std::string, PaletteRef>& getBuiltinPalettes();

}  // namespace pdft::image
