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

inline float getBaseScalingFactor(
    media::PageSize page_size, float display_width_px, float display_height_px) {
    float dw = display_width_px;
    float dh = display_height_px;
    auto [pw, ph] = page_size;
    float fw = dw / pw;
    float fh = dh / ph;
    return std::fmin(fw, fh);
}

}  // namespace pdft::media
