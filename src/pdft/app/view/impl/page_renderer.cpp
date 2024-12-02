#include "pdft/app/view/impl/page_renderer.h"

#include "pdft/tui/image/fit_image.h"

#include "pdft/image/operations.h"
#include "pdft/image/palette.h"

#include "pdft/core/trace/context.h"
#include "pdft/utils/cantor_pair.h"

namespace pdft::app::view::impl {

using tui::display::ImageId;

bool State::isLoaded(ImageId id) const {
    return loaded_.contains(id);
}

ImageId State::addImage(ImageId id, LoadedImage loaded) {
    PDFT_VERIFY(!isLoaded(id));
    loaded_.emplace(id, loaded);
    return id;
}

void State::removeImage(ImageId id) {
    PDFT_VERIFY(isLoaded(id));
    loaded_.erase(id);
}

const State::LoadedImage& State::getImage(ImageId id) const {
    PDFT_VERIFY(isLoaded(id));
    return loaded_.find(id)->second;
}

PageRenderer::PageRenderer(
    uint32_t image_id_salt,
    Arc<const state::DocumentStorage> documents,
    Arc<tui::display::Display> display)
    : image_id_salt_(image_id_salt)
    , documents_(std::move(documents))
    , display_(std::move(display)) {}

void PageRenderer::render(
    Context& ctx,
    const Page& page,
    const PagePosition& pos,
    const DisplayProperties& properties,
    tui::Screen& screen) {
    using core::trace::trace;
    const auto& [doc_id, page_index] = page;

    auto maybe_document = documents_->getDocument(page.document_id);
    PDFT_VERIFY(maybe_document);
    auto document = *maybe_document;

    auto id = getIdForPage(page);
    auto the_page = trace(ctx, "get_page", [&] { return document->getPage(page_index); });
    float factor =
        calcFactor(ctx, the_page->getDefaultSize(), properties.scaling_coeff, screen.size);

    log::debug(ctx, "rendering a page: {}, image_id={}", page, id);

    if (state_.isLoaded(id)) {
        const auto& info = state_.getImage(id);
        bool should_reload = false;

        if (factor != info.factor) {
            log::debug("loaded image factor has changed, unloading id={}", id);
            should_reload = true;
        }

        if (properties.invert_colors != info.invert_colors) {
            log::debug("loaded image color inversion has changed, unloading id={}", id);
            should_reload = true;
        }

        if ((properties.palette.has_value() && properties.palette->data() != info.palette_ptr) ||
            (!properties.palette.has_value() && info.palette_ptr != nullptr)) {
            log::debug("loaded image palette pointer has changed, unloading id={}", id);
            should_reload = true;
        }

        if (should_reload) {
            auto _ = ctx.push("unload_image");
            display_->unload(ctx, id, screen.buffers.control_buffer);
            state_.removeImage(id);
        }
    }

    if (!state_.isLoaded(id)) {
        auto _ = ctx.push("load_image");
        log::debug(ctx, "loading page to display: {}, image_id={}, factor={}", page, id, factor);

        auto image = trace(ctx, "get_image", [&] { return the_page->getImage(factor); });

        if (properties.invert_colors) {
            auto _ = ctx.push("invert_colors");
            image::invertColors(*image);
        }

        if (properties.palette.has_value()) {
            auto _ = ctx.push("apply_palette");
            image::applyPalette(image->mutableSamples(), *properties.palette);
        }

        trace(ctx, "load_image", [&] {
            display_->loadImage(ctx, id, *image, screen.buffers.control_buffer);
        });

        auto image_size = image->size();
        float image_cols = float(image_size.width_px) / screen.size.cell_width_px;
        float image_rows = float(image_size.height_px) / screen.size.cell_height_px;

        state_.addImage(
            id,
            {
                .image_rows = image_rows,
                .image_cols = image_cols,
                .factor = factor,
                .invert_colors = properties.invert_colors,
                .palette_ptr =
                    properties.palette.transform([](auto&& palette) { return palette.data(); })
                        .value_or(nullptr),
            });
    }

    auto _ = ctx.push("display_image");

    const auto& p = state_.getImage(id);
    float sub_rows = p.image_rows * pos.subarea.height;
    float sub_cols = p.image_cols * pos.subarea.width;
    auto irect = tui::image::fitImage(sub_cols / sub_rows, pos.rect, pos.placement);

    log::debug(ctx, "renderer @ {}", pos.rect);
    log::debug(ctx, "renderer I {}", irect);

    display_->displayImage(ctx, id, irect, pos.subarea, screen.buffers);
}

float PageRenderer::calcFactor(
    Context& ctx,
    media::PageSize page_size,
    float scaling_coeff,
    const tui::terminal::Dimensions& dims) {
    float dw = dims.width_px;
    float dh = dims.height_px;
    auto [pw, ph] = page_size;

    float fw = dw / pw;
    float fh = dh / ph;
    float f = std::fmin(fw, fh);

    log::debug(
        ctx,
        "calc factor for page {}x{}, term {}x{}, factor={}, scale_by={}",
        page_size.width_px,
        page_size.height_px,
        dims.width_px,
        dims.height_px,
        f,
        scaling_coeff);

    return scaling_coeff * f;
}

tui::display::ImageId PageRenderer::getIdForPage(const Page& page) const {
    auto [doc_id, page_index] = page;
    return image_id_salt_ + utils::cantorPair(doc_id, page_index + 1);
}

}  // namespace pdft::app::view::impl
