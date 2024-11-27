#include "pdft/media/mupdf/page.h"
#include "pdft/media/mupdf/pixmap_image.h"

#include "pdft/core/assert.h"
#include "pdft/core/log/log.h"

extern "C" {
#include <mupdf/fitz/archive.h>
#include <mupdf/fitz/document.h>
#include <mupdf/fitz/util.h>
}

#include <utility>

namespace pdft::media::mupdf {

Page::Page(Page&& rhs)
    : page_index_(rhs.page_index_)
    , context_(rhs.context_)
    , page_(std::exchange(rhs.page_, nullptr)) {}

Page::~Page() noexcept {
    fz_drop_page(context_.native(), page_);
}

std::shared_ptr<image::Image> Page::getImage(float scaling_factor) const {
    PDFT_VERIFY(scaling_factor > 0.f, "scale factor should be positive");

    auto ctx = context_.native();
    fz_pixmap* pix = nullptr;

    fz_matrix ctm = fz_scale(scaling_factor, scaling_factor);
    ctm = fz_pre_rotate(ctm, 0.f);

    fz_try(ctx) pix = fz_new_pixmap_from_page(ctx, page_, ctm, fz_device_rgb(ctx), /* alpha= */ 0);
    fz_catch(ctx) {
        core::fail("fz_new_pixmap_from_page");
    }

    PDFT_VERIFY(pix->w > 0 && pix->h > 0, "loaded an image with negative size");
    log::debug("loaded an image {}x{} pixels", pix->w, pix->h);

    if (pix == nullptr) {
        core::fail("fz_new_pixmap_from_page is null");
    }

    return std::shared_ptr<image::Image>(new PixmapImage(context_, pix));
}

PageSize Page::getDefaultSize() const {
    auto ctx = context_.native();
    fz_rect rect;

    fz_try(ctx) rect = fz_bound_page(ctx, page_);
    fz_catch(ctx) {
        core::fail("fz_bound_page");
    }

    return {
        .width_px = rect.x1 - rect.x0,
        .height_px = rect.y1 - rect.y0,
    };
}

size_t Page::getPageIndex() const {
    return page_index_;
}

}  // namespace pdft::media::mupdf
