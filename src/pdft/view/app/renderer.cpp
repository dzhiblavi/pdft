#include "pdft/view/app/renderer.h"

#include "pdft/core/trace/log.h"
#include "pdft/core/trace/stack_scope_tracer.h"
#include "pdft/utils/cantor_pair.h"
#include "pdft/utils/random.h"
#include "pdft/view/app/fit_page.h"
#include "pdft/view/app/fit_text.h"

namespace pdft::view::app {

Renderer::Renderer(
    core::terminal::Dimensions term_size,
    std::shared_ptr<state::DocumentStorage> docs,
    std::shared_ptr<display::Display> display,
    std::shared_ptr<core::terminal::Terminal> terminal)
    : image_id_salt_(utils::random<uint32_t>(0))
    , term_dims_(term_size)
    , docs_(std::move(docs))
    , display_(std::move(display))
    , terminal_(std::move(terminal)) {}

asio::awaitable<void> Renderer::render(
    core::io::OutBuffer& buffer,
    const IRect& rect,
    const PagePlacement& place,
    const ImageProperties& properties,
    const Page& page) {
    core::trace::StackScopeTracer trace =
        core::trace::getGlobalPool().getTracer(core::trace::stackTraceLogger(), "render_one");

    const auto& [doc_id, page_index] = page;
    const auto& document = docs_->getDocument(doc_id);
    media::ImageId id = image_id_salt_ + utils::cantorPair(doc_id, page_index + 1);

    log::debug("rendering a page: doc_id={}, page_index={}, image_id={}", doc_id, page_index, id);

    if (auto it = loaded_.find(id); it != loaded_.end() && it->second.properties != properties) {
        log::debug("loaded image properties have changed, unloading id={}", id);
        display_->unload(id, buffer);
        loaded_.erase(it);
    }

    if (!loaded_.contains(id)) {
        auto _ = trace.addChild("load_image");

        auto page = document.getPage(page_index);
        auto factor = calcFactor(page.getDefaultSize(), properties.scaling_factor);

        log::debug(
            "loading page to display: index={}, doc_id={}, image_id={}, factor={}",
            page_index,
            doc_id,
            id,
            factor);

        auto image = page.getImage({
            .scale_x = factor,
            .scale_y = factor,
            .invert_colors = properties.invert_colors,
        });

        co_await display_->loadImage(id, *image, buffer);

        auto size = image->size();
        float image_cols = float(size.width_px) / term_dims_.cell_width_px;
        float image_rows = float(size.height_px) / term_dims_.cell_height_px;

        loaded_[id] = LoadedImage{
            .cell_wh_ratio = image_cols / image_rows,
            .properties = properties,
        };
    }

    auto _ = trace.addChild("display_image");
    PDFT_VERIFY(loaded_.contains(id));
    PDFT_VERIFY(display_->isLoaded(id));

    auto [x, y, w, h] = rect;
    log::debug("renderer @ x={},y={},w={},h={}", x, y, w, h);

    auto [irect, image] = fitPage(loaded_[id].cell_wh_ratio, rect, place);
    co_await display_->displayImage(id, irect, image, buffer);
}

void Renderer::render(
    core::io::OutBuffer& buffer,
    const IRect& rect,
    const TextPlacement& placement,
    std::string_view text) {
    auto lines = fitText(rect, placement, text);

    for (auto line : lines) {
        terminal_->moveCursor(
            core::terminal::CursorPosition{
                .rows = line.y,
                .columns = line.x,
            },
            buffer);

        core::io::write(buffer, line.text);
    }
}

float Renderer::calcFactor(media::pdf::PageSize page_size, float scaling_factor) {
    float dw = term_dims_.width_px;
    float dh = term_dims_.height_px;
    auto [pw, ph] = page_size;

    float fw = dw / pw;
    float fh = dh / ph;
    float f = std::fmin(fw, fh);

    log::debug(
        "calc factor for page {}x{}, term {}x{}, factor={}, scale_by={}",
        page_size.width_px,
        page_size.height_px,
        term_dims_.width_px,
        term_dims_.height_px,
        f,
        scaling_factor);

    return scaling_factor * f;
}

}  // namespace pdft::view::app
