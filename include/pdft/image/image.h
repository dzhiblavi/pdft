#pragma once

#include <span>

namespace pdft::image {

using ImageId = uint32_t;

struct ImageProperties {
    float scaling_factor = 1.f;

    bool operator==(const ImageProperties&) const = default;
};

struct ImageSize {
    uint32_t width_px;
    uint32_t height_px;
};

class Image {
 public:
    virtual ~Image() = default;

    virtual ImageSize size() const = 0;
    virtual std::span<const uint8_t> samples() const = 0;
    virtual std::span<uint8_t> mutableSamples() = 0;
    virtual const ImageProperties& getProperties() const = 0;
};

}  // namespace pdft::image
