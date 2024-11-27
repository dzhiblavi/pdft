#include "pdft/view/app/page_renderer.h"

#include "pdft/tui/image/fit_image.h"
#include "pdft/tui/image/image.h"

#include "pdft/utils/cantor_pair.h"

namespace pdft::view::app {

PageRenderer::PageRenderer(
    uint32_t image_id_salt,
    std::shared_ptr<state::DocumentStorage> docs,
    std::shared_ptr<tui::display::State> display_state,
    std::shared_ptr<tui::display::Display> display)
    : image_id_salt_(image_id_salt)
    , docs_(std::move(docs))
    , display_state_(std::move(display_state))
    , display_(std::move(display)) {}

void PageRenderer::render(const Page& page, const PageProperties& properties, tui::Context& ctx) {
    const auto& [doc_id, page_index] = page;
    const auto& document = docs_->getDocument(page.document_id);
    const auto& props = properties.properties;
    auto id = getIdForPage(page);

    log::debug(
        ctx, "rendering a page: doc_id={}, page_index={}, image_id={}", doc_id, page_index, id);

    if (display_state_->isLoaded(id)) {
        const auto& info = display_state_->getImage(id);
        bool should_reload = false;

        if (info.properties.scaling_factor != props.scaling_factor) {
            log::debug("loaded image scaling factor has changed, unloading id={}", id);
            should_reload = true;
        }

        if (props.invert_colors != info.properties.invert_colors) {
            log::debug("loaded image color inversion has changed, unloading id={}", id);
            should_reload = true;
        }

        if (should_reload) {
            auto _ = ctx.push("unload_image");
            display_->unload(id, ctx.buffer);
            display_state_->removeImage(id);
        }
    }

    if (!display_state_->isLoaded(id)) {
        auto _ = ctx.push("load_image");

        auto page = document.getPage(page_index);
        auto page_size = page->getDefaultSize();
        auto factor = calcFactor(page_size, props.scaling_factor, ctx.screen_size, ctx);

        log::debug(
            ctx,
            "loading page to display: index={}, doc_id={}, image_id={}, factor={}",
            page_index,
            doc_id,
            id,
            factor);

        auto image = page->getImage({
            .scaling_factor = factor,
            .invert_colors = props.invert_colors,
        });

        display_->loadImage(id, *image, ctx.buffer);

        auto image_size = image->size();
        float image_cols = float(image_size.width_px) / ctx.screen_size.cell_width_px;
        float image_rows = float(image_size.height_px) / ctx.screen_size.cell_height_px;

        display_state_->addImage(
            {
                .image_rows = image_rows,
                .image_cols = image_cols,
                .properties = properties.properties,
            },
            id);
    }

    auto _ = ctx.push("display_image");

    const auto& p = display_state_->getImage(id);
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

float PageRenderer::calcFactor(
    media::PageSize page_size,
    float scaling_factor,
    const tui::terminal::Dimensions& screen_size,
    tui::Context& ctx) {
    float dw = screen_size.width_px;
    float dh = screen_size.height_px;
    auto [pw, ph] = page_size;

    float fw = dw / pw;
    float fh = dh / ph;
    float f = std::fmin(fw, fh);

    log::debug(
        ctx,
        "calc factor for page {}x{}, term {}x{}, factor={}, scale_by={}",
        page_size.width_px,
        page_size.height_px,
        screen_size.width_px,
        screen_size.height_px,
        f,
        scaling_factor);

    return scaling_factor * f;
}

image::ImageId PageRenderer::getIdForPage(const Page& page) const {
    auto [doc_id, page_index] = page;
    return image_id_salt_ + utils::cantorPair(doc_id, page_index + 1);
}

}  // namespace pdft::view::app
