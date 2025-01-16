#include "pdft/media/mupdf/page.h"
#include "pdft/media/mupdf/context.h"
#include "pdft/media/mupdf/pixmap_image.h"

#include "fz_assert.h"

#include "platform/core/assert.h"

extern "C" {
#include <mupdf/fitz/archive.h>
#include <mupdf/fitz/document.h>
#include <mupdf/fitz/util.h>
}

#include <utility>

namespace pdft::media {

MupdfPage::MupdfPage(MupdfPage&& rhs) noexcept
    : page_index_(rhs.page_index_)
    , page_(std::exchange(rhs.page_, nullptr)) {}

MupdfPage::~MupdfPage() noexcept {
    fz_drop_page(thisThreadMupdfContext().native(), page_);
}

platform::Arc<image::Image> MupdfPage::getImage(float scaling_factor) const {
    platform::verify(scaling_factor > 0.f, "scale factor should be positive");

    auto ctx = thisThreadMupdfContext().native();
    fz_matrix ctm = fz_scale(scaling_factor, scaling_factor);

    auto pix = tryFz(ctx, [&] {
        return fz_new_pixmap_from_page(ctx, page_, ctm, fz_device_rgb(ctx), /* alpha= */ 0);
    });

    platform::verify(pix, "failed to get pixmap by page");
    platform::verify((*pix)->w > 0 && (*pix)->h > 0, "loaded an image with negative size");
    return platform::Arc<image::Image>(new PixmapMupdfImage(*pix));
}

PageSize MupdfPage::getDefaultSize() const {
    auto ctx = thisThreadMupdfContext().native();
    auto maybe_rect = tryFz(ctx, [&] { return fz_bound_page(ctx, page_); });
    platform::verify(maybe_rect, "failed to get default size for page");
    fz_rect rect = *maybe_rect;

    return {
        .width_px = rect.x1 - rect.x0,
        .height_px = rect.y1 - rect.y0,
    };
}

size_t MupdfPage::getPageIndex() const {
    return page_index_;
}

}  // namespace pdft::media
