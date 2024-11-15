#include "pdft/view/app/renderer.h"

#include "pdft/core/trace/log_duration.h"
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
    core::trace::StackScopeTracer trace_render_one(core::trace::makeLoggingTracer("render_one"));

    const auto& [pos, doc_id, page_index] = layout;
    const auto& document = docs_->getDocument(doc_id);
    display::ImageId id = utils::cantorPair(doc_id, page_index + 1);

    GLOG(info, "rendering a page: doc_id={}, page_index={}, image_id={}", doc_id, page_index, id);

    if (!display_->isLoaded(id)) {
        core::trace::StackScopeTracer _(
            trace_render_one, core::trace::makeLoggingTracer("display_load_one"));

        auto page = document.getPage(page_index);
        auto factor = calcFactor(page.getDefaultSize());

        GLOG(
            info,
            "loading page to display: index={}, doc_id={}, image_id={}, factor={}",
            page_index,
            doc_id,
            id,
            factor);

        auto image = page.getImage({
            .scale_x = factor,
            .scale_y = factor,
        });
        display_->loadImage(id, *image, buffer);
    }

    core::trace::StackScopeTracer _(
        trace_render_one, core::trace::makeLoggingTracer("display_display_one"));

    display_->displayImage(id, pos, buffer);
}

}  // namespace pdft::view::app
