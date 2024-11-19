#pragma once

#include <string_view>

namespace pdft::media {

using ImageId = uint32_t;

struct ImageSize {
    int width_px;
    int height_px;
};

class Image {
 public:
    virtual ~Image() = default;

    virtual ImageSize size() const = 0;
    virtual std::string_view samples() const = 0;
};

}  // namespace pdft::media
