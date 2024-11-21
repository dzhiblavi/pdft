#include "pdft/media/pdf/pixmap_image.h"
#include "pdft/core/assert.h"

extern "C" {
#include <mupdf/fitz/pixmap.h>
}

#include <utility>

namespace pdft::media::pdf {

PixmapImage::PixmapImage(PixmapImage&& rhs)
    : context_(rhs.context_)
    , pixmap_(std::exchange(rhs.pixmap_, nullptr)) {
    assert(pixmap_);
}

PixmapImage::~PixmapImage() noexcept {
    if (pixmap_ == nullptr) {
        return;
    }

    /*
       [...]
       Bit 1: If set, free the samples buffer when the pixmap is destroyed.
    */
    // if (pixmap_->flags & (1 << 1)) {
    //::free(pixmap_->samples);
    //}

    fz_drop_pixmap(context_.native(), pixmap_);
}

ImageSize PixmapImage::size() const {
    PDFT_VERIFY(pixmap_ != nullptr, "invalid pixmap image");

    return {
        .width_px = static_cast<uint32_t>(pixmap_->w),
        .height_px = static_cast<uint32_t>(pixmap_->h),
    };
}

std::string_view PixmapImage::samples() const {
    PDFT_VERIFY(pixmap_ != nullptr, "invalid pixmap image");

    return {
        (char*)pixmap_->samples,
        size_t(pixmap_->w * pixmap_->h * pixmap_->n),
    };
}

}  // namespace pdft::media::pdf
