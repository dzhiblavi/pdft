#include "pdft/media/mupdf/pixmap_image.h"
#include "pdft/media/mupdf/context.h"

#include "platform/core/assert.h"

#include "platform/utils/stdlike/memory.h"

extern "C" {
#include <mupdf/fitz/pixmap.h>
}

#include <utility>

namespace pdft::media {

PixmapMupdfImage::PixmapMupdfImage(PixmapMupdfImage&& rhs) noexcept
    : pixmap_(platform::verify(std::exchange(rhs.pixmap_, nullptr))) {}

PixmapMupdfImage::~PixmapMupdfImage() noexcept {
    if (pixmap_ == nullptr) {
        return;
    }

    fz_drop_pixmap(thisThreadMupdfContext().native(), pixmap_);
}

image::ImageSize PixmapMupdfImage::size() const {
    return {
        .width_px = static_cast<uint32_t>(pixmap_->w),
        .height_px = static_cast<uint32_t>(pixmap_->h),
    };
}

std::span<const image::IRGBColor> PixmapMupdfImage::samples() const {
    size_t pixels_count = pixmap_->w * pixmap_->h;
    return {
        platform::stdlike::start_lifetime_as_array<const image::IRGBColor>(
            pixmap_->samples, pixels_count),
        pixels_count,
    };
}

std::span<image::IRGBColor> PixmapMupdfImage::mutableSamples() {
    size_t pixels_count = pixmap_->w * pixmap_->h;
    return {
        platform::stdlike::start_lifetime_as_array<image::IRGBColor>(
            pixmap_->samples, pixels_count),
        pixels_count,
    };
}

}  // namespace pdft::media
