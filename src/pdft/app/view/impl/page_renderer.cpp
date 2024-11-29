#include "pdft/app/view/impl/page_renderer.h"

#include "pdft/tui/image/fit_image.h"
#include "pdft/tui/image/image.h"

#include "pdft/image/invert_colors.h"

#include "pdft/core/trace/context.h"
#include "pdft/utils/cantor_pair.h"

namespace pdft::app::view::impl {

PageRenderer::PageRenderer(
    uint32_t image_id_salt,
    std::shared_ptr<const state::DocumentStorage> documents,
    std::shared_ptr<tui::display::Display> display)
    : image_id_salt_(image_id_salt)
    , documents_(std::move(documents))
    , display_(std::move(display)) {}

void PageRenderer::render(
    const Page& page,
    const PageProperties& properties,
    tui::display::State& display,
    tui::Context& ctx) {
    const auto& [doc_id, page_index] = page;
    const auto& document = documents_->getDocument(page.document_id);

    auto id = getIdForPage(page);
    auto the_page =
        core::trace::trace(ctx, "get_page", [&] { return document.getPage(page_index); });
    float factor = calcFactor(the_page->getDefaultSize(), properties.scaling_coeff, ctx);

    log::debug(
        ctx, "rendering a page: doc_id={}, page_index={}, image_id={}", doc_id, page_index, id);

    if (display.isLoaded(id)) {
        const auto& info = display.getImage(id);
        bool should_reload = false;

        if (factor != info.factor) {
            log::debug("loaded image factor has changed, unloading id={}", id);
            should_reload = true;
        }

        if (properties.invert_colors != info.invert_colors) {
            log::debug("loaded image color inversion has changed, unloading id={}", id);
            should_reload = true;
        }

        if (should_reload) {
            auto _ = ctx.push("unload_image");
            display_->unload(id, ctx.control_buffer);
            display.removeImage(id);
        }
    }

    if (!display.isLoaded(id)) {
        auto _ = ctx.push("load_image");

        log::debug(
            ctx,
            "loading page to display: index={}, doc_id={}, image_id={}, factor={}",
            page_index,
            doc_id,
            id,
            factor);

        auto image =
            core::trace::trace(ctx, "get_image", [&] { return the_page->getImage(factor); });

        if (properties.invert_colors) {
            image::invertColors(*image);
        }

        display_->loadImage(id, *image, ctx.control_buffer);

        auto image_size = image->size();
        float image_cols = float(image_size.width_px) / ctx.screen_size.cell_width_px;
        float image_rows = float(image_size.height_px) / ctx.screen_size.cell_height_px;

        display.addImage(
            id,
            {
                .image_rows = image_rows,
                .image_cols = image_cols,
                .factor = factor,
                .invert_colors = properties.invert_colors,
            });
    }

    auto _ = ctx.push("display_image");

    const auto& p = display.getImage(id);
    float sub_rows = p.image_rows * properties.subarea.height;
    float sub_cols = p.image_cols * properties.subarea.width;
    auto irect = tui::image::fitImage(sub_cols / sub_rows, properties.rect, properties.placement);
    auto image = tui::image::Image::make(id, properties.subarea);

    {
        auto [x, y, w, h] = properties.rect;
        log::debug(ctx, "renderer @ x={},y={},w={},h={}", x, y, w, h);

        auto [ix, iy, iw, ih] = irect;
        log::debug(ctx, "renderer I ix={},iy={},iw={},ih={}", ix, iy, iw, ih);
    }

    tui::image::render(image, irect, *display_, ctx);
}

float PageRenderer::calcFactor(media::PageSize page_size, float scaling_coeff, tui::Context& ctx) {
    float dw = ctx.screen_size.width_px;
    float dh = ctx.screen_size.height_px;
    auto [pw, ph] = page_size;

    float fw = dw / pw;
    float fh = dh / ph;
    float f = std::fmin(fw, fh);

    log::debug(
        ctx,
        "calc factor for page {}x{}, term {}x{}, factor={}, scale_by={}",
        page_size.width_px,
        page_size.height_px,
        ctx.screen_size.width_px,
        ctx.screen_size.height_px,
        f,
        scaling_coeff);

    return scaling_coeff * f;
}

tui::display::ImageId PageRenderer::getIdForPage(const Page& page) const {
    auto [doc_id, page_index] = page;
    return image_id_salt_ + utils::cantorPair(doc_id, page_index + 1);
}

}  // namespace pdft::app::view::impl
