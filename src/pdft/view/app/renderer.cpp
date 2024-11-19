#include "pdft/view/app/renderer.h"

#include "pdft/core/trace/log.h"
#include "pdft/core/trace/stack_scope_tracer.h"
#include "pdft/utils/cantor_pair.h"
#include "pdft/view/app/fit_image.h"

namespace pdft::view::app {

void Renderer::render(core::OutBuffer& buffer, const PagePlacement& layout) {
    core::trace::StackScopeTracer trace =
        core::trace::getGlobalPool().getTracer(core::trace::stackTraceLogger(), "render_one");

    display_->clear(buffer);

    const auto& [pos, doc_id, page_index] = layout;
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
        auto [width, height] = image_size_cells_[id];
        display_->displayImage(id, fitImage(width, height, pos), buffer);
    }
}

void Renderer::render(core::OutBuffer& buffer, const TextPlacement& placement) {
    terminal_->moveCursor(
        core::terminal::CursorPosition{
            .rows = static_cast<uint32_t>(placement.position.rect.y + 1),
            .columns = static_cast<uint32_t>(placement.position.rect.x + 1),
        },
        buffer);

    core::write(buffer, placement.text);
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
