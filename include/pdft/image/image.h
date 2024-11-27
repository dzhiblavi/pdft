#pragma once

#include "pdft/image/size.h"

#include <span>

namespace pdft::image {

class Image {
 public:
    virtual ~Image() = default;

    virtual ImageSize size() const = 0;
    virtual std::span<const uint8_t> samples() const = 0;
    virtual std::span<uint8_t> mutableSamples() = 0;
};

}  // namespace pdft::image
