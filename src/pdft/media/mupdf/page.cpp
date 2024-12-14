#include "pdft/media/mupdf/page.h"
#include "pdft/media/mupdf/context.h"
#include "pdft/media/mupdf/pixmap_image.h"

#include "fz_assert.h"

#include "pdft/core/assert.h"

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
    fz_drop_page(MupdfContext::thisThreadInstance().native(), page_);
}

Arc<image::Image> MupdfPage::getImage(float scaling_factor) const {
    PDFT_VERIFY(scaling_factor > 0.f, "scale factor should be positive");

    auto ctx = MupdfContext::thisThreadInstance().native();
    fz_pixmap* pix = nullptr;
    fz_matrix ctm = fz_scale(scaling_factor, scaling_factor);

    PDFT_FZ_VERIFY(
        pix = fz_new_pixmap_from_page(ctx, page_, ctm, fz_device_rgb(ctx), /* alpha= */ 0));
    PDFT_VERIFY(pix->w > 0 && pix->h > 0, "loaded an image with negative size");
    PDFT_VERIFY(pix, "fz_new_pixmap_from_page is null");

    return Arc<image::Image>(new PixmapMupdfImage(pix));
}

PageSize MupdfPage::getDefaultSize() const {
    auto ctx = MupdfContext::thisThreadInstance().native();
    fz_rect rect;

    PDFT_FZ_VERIFY(rect = fz_bound_page(ctx, page_));

    return {
        .width_px = rect.x1 - rect.x0,
        .height_px = rect.y1 - rect.y0,
    };
}

size_t MupdfPage::getPageIndex() const {
    return page_index_;
}

}  // namespace pdft::media
