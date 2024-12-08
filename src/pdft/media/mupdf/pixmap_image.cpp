#include "pdft/media/mupdf/pixmap_image.h"
#include "pdft/media/mupdf/context.h"

#include "pdft/core/assert.h"

extern "C" {
#include <mupdf/fitz/pixmap.h>
}

#include <utility>

namespace pdft::media {

PixmapMupdfImage::PixmapMupdfImage(PixmapMupdfImage&& rhs)
    : pixmap_(PDFT_VERIFY(std::exchange(rhs.pixmap_, nullptr))) {}

PixmapMupdfImage::~PixmapMupdfImage() noexcept {
    if (pixmap_ == nullptr) {
        return;
    }

    fz_drop_pixmap(MupdfContext::thisThreadInstance().native(), pixmap_);
}

image::ImageSize PixmapMupdfImage::size() const {
    return {
        .width_px = static_cast<uint32_t>(pixmap_->w),
        .height_px = static_cast<uint32_t>(pixmap_->h),
    };
}

std::span<const image::RGBColor> PixmapMupdfImage::samples() const {
    return {
        (const image::RGBColor*)pixmap_->samples,
        size_t(pixmap_->w * pixmap_->h),
    };
}

std::span<image::RGBColor> PixmapMupdfImage::mutableSamples() {
    return {
        (image::RGBColor*)pixmap_->samples,
        size_t(pixmap_->w * pixmap_->h),
    };
}

}  // namespace pdft::media
