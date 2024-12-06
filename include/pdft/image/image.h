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

inline std::string_view pixelsToSamples(std::span<const RGBColor> pixels) {
    static_assert(sizeof(RGBColor) == 3);

    return {
        (const char*)pixels.data(),
        sizeof(RGBColor) * pixels.size(),
    };
}

}  // namespace pdft::image
