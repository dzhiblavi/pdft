#pragma once

#include "pdft/image/color.h"
#include "pdft/image/size.h"

#include <span>

namespace pdft::image {

class Image {
 public:
    virtual ~Image() = default;

    virtual ImageSize size() const = 0;
    virtual std::span<const IRGBColor> samples() const = 0;
    virtual std::span<IRGBColor> mutableSamples() = 0;
};

inline std::string_view pixelsToSamples(std::span<const IRGBColor> pixels) {
    static_assert(sizeof(IRGBColor) == 3);

    return {
        // It is legal to observe byte representation of any object.
        reinterpret_cast<const char*>(pixels.data()),  // NOLINT
        sizeof(IRGBColor) * pixels.size(),
    };
}

}  // namespace pdft::image
