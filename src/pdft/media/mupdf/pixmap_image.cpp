#include "pdft/media/mupdf/pixmap_image.h"
#include "pdft/core/assert.h"

extern "C" {
#include <mupdf/fitz/pixmap.h>
}

#include <utility>

namespace pdft::media {

PixmapMupdfImage::PixmapMupdfImage(PixmapMupdfImage&& rhs)
    : context_(rhs.context_)
    , pixmap_(std::exchange(rhs.pixmap_, nullptr)) {
    PDFT_VERIFY(pixmap_);
}

PixmapMupdfImage::~PixmapMupdfImage() noexcept {
    if (pixmap_ == nullptr) {
        return;
    }

    fz_drop_pixmap(context_.native(), pixmap_);
}

image::ImageSize PixmapMupdfImage::size() const {
    PDFT_VERIFY(pixmap_ != nullptr, "invalid pixmap image");

    return {
        .width_px = static_cast<uint32_t>(pixmap_->w),
        .height_px = static_cast<uint32_t>(pixmap_->h),
    };
}

std::span<const uint8_t> PixmapMupdfImage::samples() const {
    PDFT_VERIFY(pixmap_ != nullptr, "invalid pixmap image");

    return {
        (const uint8_t*)pixmap_->samples,
        size_t(pixmap_->w * pixmap_->h * pixmap_->n),
    };
}

std::span<uint8_t> PixmapMupdfImage::mutableSamples() {
    PDFT_VERIFY(pixmap_ != nullptr, "invalid pixmap image");

    return {
        (uint8_t*)pixmap_->samples,
        size_t(pixmap_->w * pixmap_->h * pixmap_->n),
    };
}

}  // namespace pdft::media
