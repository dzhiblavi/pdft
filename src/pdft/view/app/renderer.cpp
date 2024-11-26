#include "pdft/view/app/renderer.h"

#include "pdft/core/trace/make_stack_tracer_logger.h"
#include "pdft/core/trace/stack_scope_tracer.h"
#include "pdft/utils/cantor_pair.h"
#include "pdft/utils/random.h"

#include "pdft/tui/image/fit_image.h"
#include "pdft/tui/image/image.h"

namespace pdft::view::app {

Renderer::Renderer(
    std::shared_ptr<state::DocumentStorage> docs, std::shared_ptr<tui::display::Display> display)
    : image_id_salt_(utils::random<uint32_t>(0))
    , docs_(std::move(docs))
    , display_(std::move(display)) {}

void Renderer::render(
    const tui::IRect& rect,
    const tui::FRect& subarea,
    const tui::image::Placement& place,
    const image::ImageProperties& properties,
    const Page& page,
    tui::Context& ctx) {
    auto _ = ctx.push("render_one");

    const auto& [doc_id, page_index] = page;
    const auto& document = docs_->getDocument(doc_id);
    auto id = image_id_salt_ + utils::cantorPair(doc_id, page_index + 1);

    log::debug("rendering a page: doc_id={}, page_index={}, image_id={}", doc_id, page_index, id);

    if (auto it = loaded_.find(id); it != loaded_.end()) {
        auto&& info = it->second;

        bool should_reload = false;
        auto factor = calcFactor(info.page_size, properties.scaling_factor, ctx.screen_size);

        if (factor != info.factor) {
            log::debug("loaded image scaling factor has changed, unloading id={}", id);
            should_reload = true;
        }

        if (properties.invert_colors != info.inverted_colors) {
            log::debug("loaded image color inversion has changed, unloading id={}", id);
            should_reload = true;
        }

        if (should_reload) {
            auto _ = ctx.push("unload_image");
            display_->unload(id, ctx.buffer);
            loaded_.erase(it);
        }
    }

    if (!loaded_.contains(id)) {
        auto _ = ctx.push("load_image");

        auto page = document.getPage(page_index);
        auto page_size = page->getDefaultSize();
        auto factor = calcFactor(page_size, properties.scaling_factor, ctx.screen_size);

        log::debug(
            "loading page to display: index={}, doc_id={}, image_id={}, factor={}",
            page_index,
            doc_id,
            id,
            factor);

        auto image = page->getImage({
            .scaling_factor = factor,
            .invert_colors = properties.invert_colors,
        });

        display_->loadImage(id, *image, ctx.buffer);

        auto image_size = image->size();
        float image_cols = float(image_size.width_px) / ctx.screen_size.cell_width_px;
        float image_rows = float(image_size.height_px) / ctx.screen_size.cell_height_px;

        loaded_[id] = LoadedImage{
            .page_size = page_size,
            .inverted_colors = properties.invert_colors,
            .factor = factor,
            .image_rows = image_rows,
            .image_cols = image_cols,
        };
    }

    auto _2 = ctx.push("display_image");
    PDFT_VERIFY(loaded_.contains(id));
    PDFT_VERIFY(display_->isLoaded(id));

    const auto& props = loaded_[id];
    float sub_rows = props.image_rows * subarea.height;
    float sub_cols = props.image_cols * subarea.width;
    auto irect = tui::image::fitImage(sub_cols / sub_rows, rect, place);
    auto image = tui::image::Image::make(id, subarea);

    {
        auto [x, y, w, h] = rect;
        log::debug("renderer @ x={},y={},w={},h={}", x, y, w, h);

        auto [ix, iy, iw, ih] = irect;
        log::debug("renderer I ix={},iy={},iw={},ih={}", ix, iy, iw, ih);
    }

    tui::image::render(image, irect, *display_, ctx);
}

float Renderer::calcFactor(
    media::PageSize page_size, float scaling_factor, const tui::terminal::Dimensions& screen_size) {
    float dw = screen_size.width_px;
    float dh = screen_size.height_px;
    auto [pw, ph] = page_size;

    float fw = dw / pw;
    float fh = dh / ph;
    float f = std::fmin(fw, fh);

    log::debug(
        "calc factor for page {}x{}, term {}x{}, factor={}, scale_by={}",
        page_size.width_px,
        page_size.height_px,
        screen_size.width_px,
        screen_size.height_px,
        f,
        scaling_factor);

    return scaling_factor * f;
}

}  // namespace pdft::view::app
