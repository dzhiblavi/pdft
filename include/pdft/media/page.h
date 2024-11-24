#pragma once

#include "pdft/display/image.h"

#include <memory>

namespace pdft::media {

struct PageSize {
    float width_px;
    float height_px;
};

class Page {
 public:
    virtual ~Page() = default;

    virtual std::unique_ptr<display::Image> getImage(
        const display::ImageProperties& properties) const = 0;

    virtual PageSize getDefaultSize() const = 0;
    virtual size_t getPageIndex() const = 0;
};

}  // namespace pdft::media
