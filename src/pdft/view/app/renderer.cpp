#include "pdft/view/app/renderer.h"

#include "pdft/core/trace/log.h"
#include "pdft/core/trace/stack_scope_tracer.h"
#include "pdft/utils/cantor_pair.h"
#include "pdft/view/app/fit_page.h"
#include "pdft/view/app/fit_text.h"

namespace pdft::view::app {

void Renderer::render(
    core::io::OutBuffer& buffer, const IRect& rect, const PagePlacement& place, const Page& page) {
    core::trace::StackScopeTracer trace =
        core::trace::getGlobalPool().getTracer(core::trace::stackTraceLogger(), "render_one");

    display_->clear(buffer);

    const auto& [doc_id, page_index] = page;
    const auto& document = docs_->getDocument(doc_id);
    media::ImageId id = utils::cantorPair(doc_id, page_index + 1);

    core::log::global()->info(
        "rendering a page: doc_id={}, page_index={}, image_id={}", doc_id, page_index, id);

    if (!display_->isLoaded(id)) {
        auto _ = trace.addChild("load_image");

        auto page = document.getPage(page_index);
        auto factor = calcFactor(page.getDefaultSize());

        core::log::global()->info(
            "loading page to display: index={}, doc_id={}, image_id={}, factor={}",
            page_index,
            doc_id,
            id,
            factor);

        auto image = page.getImage({
            .scale_x = factor,
            .scale_y = factor,
            .invert_colors = false,
        });

        display_->loadImage(id, *image, buffer);

        auto size = image->size();
        float image_cols = float(size.width_px) / term_dims_.cell_width_px;
        float image_rows = float(size.height_px) / term_dims_.cell_height_px;
        image_size_cells_[id] = std::make_pair(image_cols, image_rows);
    }

    {
        auto _ = trace.addChild("display_image");
        PDFT_VERIFY(image_size_cells_.contains(id));

        auto [x, y, w, h] = rect;
        core::log::global()->info("renderer @ x={},y={},w={},h={}", x, y, w, h);

        auto [width, height] = image_size_cells_[id];
        auto [irect, image] = fitPage(width, height, rect, place);
        display_->displayImage(id, irect, image, buffer);
    }
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
                .rows = line.y + 1,
                .columns = line.x + 1,
            },
            buffer);

        core::io::write(buffer, line.text);
    }
}

float Renderer::calcFactor(media::pdf::PageSize page_size) {
    core::log::global()->info(
        "calc factor for page {}x{}, term {}x{}",
        page_size.width_px,
        page_size.height_px,
        term_dims_.width_px,
        term_dims_.height_px);

    float dw = term_dims_.width_px;
    float dh = term_dims_.height_px;
    auto [pw, ph] = page_size;

    float fw = dw / pw;
    float fh = dh / ph;

    return std::min(fw, fh);
}

}  // namespace pdft::view::app
