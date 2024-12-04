#pragma once

#include "pdft/image/color.h"
#include "pdft/image/size.h"

#include <span>

namespace pdft::image {

class Image {
 public:
    virtual ~Image() = default;

    virtual ImageSize size() const = 0;
    virtual std::span<const RGBColor> samples() const = 0;
    virtual std::span<RGBColor> mutableSamples() = 0;
};

}  // namespace pdft::image
