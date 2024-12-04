#pragma once

#include "pdft/image/image.h"

#include "pdft/core/rc.h"

namespace pdft::media {

struct PageSize {
    float width_px;
    float height_px;
};

class Page {
 public:
    virtual ~Page() = default;

    virtual Arc<image::Image> getImage(float scaling_factor) const = 0;
    virtual PageSize getDefaultSize() const = 0;
    virtual size_t getPageIndex() const = 0;
};

}  // namespace pdft::media
