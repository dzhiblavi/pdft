#pragma once

#include <string_view>

namespace pdft::display {

using ImageId = uint32_t;

struct ImageProperties {
    float scaling_factor = 1.f;
    bool invert_colors = false;

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
    virtual std::string_view samples() const = 0;
};

}  // namespace pdft::display
