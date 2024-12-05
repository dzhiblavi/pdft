#pragma once

#include "pdft/image/image.h"

namespace pdft::image {

void invertColors(std::span<RGBColor> pixels);

void makeBolder(Image& image);

}  // namespace pdft::image
