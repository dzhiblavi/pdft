#include "pdft/view/app/renderer.h"

#include "pdft/core/trace/log.h"
#include "pdft/core/trace/stack_scope_tracer.h"
#include "pdft/utils/cantor_pair.h"

namespace pdft::view::app {

void Renderer::render(core::OutBuffer& buffer, const std::vector<Placement>& layout) {
    for (const auto& placement : layout) {
        renderOne(buffer, placement);
    }
}

float Renderer::calcFactor(media::pdf::PageSize page_size) {
    float dw = term_dims_.width_px;
    float dh = term_dims_.height_px;
    auto [pw, ph] = page_size;

    float fw = dw / pw;
    float fh = dh / ph;

    return std::min(fw, fh);
}

void Renderer::renderOne(core::OutBuffer& buffer, const Placement& layout) {
    core::trace::StackScopeTracer trace =
        core::trace::getGlobalPool().getTracer(core::trace::stackTraceLogger(), "render_one");

    const auto& [pos, doc_id, page_index] = layout;
    const auto& document = docs_->getDocument(doc_id);
    display::ImageId id = utils::cantorPair(doc_id, page_index + 1);

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
    }

    {
        auto _ = trace.addChild("display_image");
        display_->displayImage(id, pos, buffer);
    }
}

}  // namespace pdft::view::app
