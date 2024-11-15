#include "pdft/media/pdf/page.h"
#include "pdft/core/assert.h"
#include "pdft/media/pdf/pixmap_image.h"

extern "C" {
#include <mupdf/fitz/archive.h>
#include <mupdf/fitz/document.h>
#include <mupdf/fitz/util.h>
}

#include <utility>

namespace pdft::media::pdf {

Page::Page(Page&& rhs) : context_(rhs.context_), page_(std::exchange(rhs.page_, nullptr)) {}

Page::~Page() noexcept {
    fz_drop_page(context_.native(), page_);
}

std::unique_ptr<Image> Page::getImage(const ImageProperties& properties) {
    auto ctx = context_.native();
    fz_pixmap* page = nullptr;

    fz_matrix ctm = fz_scale(properties.scale_x, properties.scale_y);
    ctm = fz_pre_rotate(ctm, properties.rotation_degrees);

    fz_try(ctx) page = fz_new_pixmap_from_page(ctx, page_, ctm, fz_device_rgb(ctx), /* alpha= */ 0);

    fz_catch(ctx) {
        core::fail("fz_new_pixmap_from_page");
    }

    if (page == nullptr) {
        core::fail("fz_new_pixmap_from_page is null");
    }

    return std::unique_ptr<Image>(new PixmapImage(context_, page));
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

}  // namespace pdft::media::pdf
