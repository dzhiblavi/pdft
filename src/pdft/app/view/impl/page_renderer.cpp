#include "pdft/app/view/impl/page_renderer.h"

#include "pdft/image/operations.h"
#include "pdft/image/palette.h"

#include "pdft/core/result/combine/unwrap.h"
#include "pdft/core/result/syntax/pipe.h"
#include "pdft/core/trace/context.h"

#include "pdft/utils/cantor_pair.h"

namespace pdft::app {

using tui::ImageId;

bool AppPageRenderer::State::isLoaded(ImageId id) const {
    return loaded_.contains(id);
}

ImageId AppPageRenderer::State::addImage(ImageId id, LoadedImage loaded) {
    PDFT_VERIFY(!isLoaded(id));
    loaded_.emplace(id, loaded);
    return id;
}

void AppPageRenderer::State::removeImage(ImageId id) {
    PDFT_VERIFY(isLoaded(id));
    loaded_.erase(id);
}

const AppPageRenderer::LoadedImage& AppPageRenderer::State::getImage(ImageId id) const {
    PDFT_VERIFY(isLoaded(id));
    return loaded_.find(id)->second;
}

AppPageRenderer::AppPageRenderer(
    uint32_t image_id_salt, Arc<const DocumentStorage> documents, Arc<tui::Display> display)
    : image_id_salt_(image_id_salt)
    , documents_(std::move(documents))
    , display_(std::move(display)) {}

void AppPageRenderer::render(
    Context& ctx,
    const Page& page,
    const PagePosition& pos,
    const DisplayProperties& properties,
    tui::Screen& screen) {
    using trace::trace;
    const auto& [doc_id, page_index] = page;

    auto document = documents_->getDocument(page.document_id) | result::unwrap;
    auto id = getIdForPage(page);
    auto the_page = trace(ctx, "get_page", [&] { return document->getPage(page_index); });
    float factor =
        calcFactor(ctx, the_page->getDefaultSize(), properties.scaling_coeff, screen.size);

    log::debug(ctx, "rendering a page: {}, image_id={}", page, id);

    if (state_.isLoaded(id) && shouldReload(id, factor, properties)) {
        auto _ = ctx.push("unload_image");
        display_->unload(ctx, id, screen.buffers.control_buffer);
        state_.removeImage(id);
    }

    if (!state_.isLoaded(id)) {
        auto image = trace::trace(ctx, "get_image", [&] { return the_page->getImage(factor); });
        auto _ = ctx.push("load_image");
        loadImage(ctx, id, *image, factor, properties, screen);
    }

    const auto& p = state_.getImage(id);
    float sub_rows = p.image_rows * pos.subarea.height;
    float sub_cols = p.image_cols * pos.subarea.width;
    auto irect = tui::fitImage(sub_cols / sub_rows, pos.rect, pos.placement);

    log::debug(ctx, "renderer @{} I{}", pos.rect, irect);
    auto _ = ctx.push("display_image");
    display_->displayImage(ctx, id, irect, pos.subarea, screen.buffers);
}

bool AppPageRenderer::shouldReload(tui::ImageId id, float factor, const DisplayProperties& prop) {
    const auto& info = state_.getImage(id);

    if (factor != info.factor) {
        log::debug("loaded image factor has changed, unloading id={}", id);
        return true;
    }

    if (prop.invert_colors != info.invert_colors) {
        log::debug("loaded image color inversion has changed, unloading id={}", id);
        return true;
    }

    if (prop.make_bolder != info.make_bolder) {
        log::debug("loaded image make_bolder has changed, unloading id={}", id);
        return true;
    }

    if ((prop.palette.has_value() && prop.palette->data() != info.palette_ptr) ||
        (!prop.palette.has_value() && info.palette_ptr != nullptr)) {
        log::debug("loaded image palette pointer has changed, unloading id={}", id);
        return true;
    }

    return false;
}

void AppPageRenderer::loadImage(
    Context& ctx,
    tui::ImageId id,
    image::Image& image,
    float factor,
    const DisplayProperties& prop,
    tui::Screen& screen) {
    if (prop.invert_colors) {
        auto _ = ctx.push("invert_colors");
        image::invertColors(image.mutableSamples());
    }

    if (prop.make_bolder) {
        auto _ = ctx.push("make_bolder");
        image::makeBolder(image);
    }

    if (prop.palette.has_value()) {
        auto _ = ctx.push("apply_palette");
        image::applyPalette(image.mutableSamples(), *prop.palette);
    }

    trace::trace(ctx, "load_image", [&] {
        display_->loadImage(ctx, id, image, screen.buffers.control_buffer);
    });

    auto image_size = image.size();
    float image_cols = float(image_size.width_px) / screen.size.cell_width_px;
    float image_rows = float(image_size.height_px) / screen.size.cell_height_px;

    state_.addImage(
        id,
        {
            .image_rows = image_rows,
            .image_cols = image_cols,
            .factor = factor,
            .invert_colors = prop.invert_colors,
            .make_bolder = prop.make_bolder,
            .palette_ptr = prop.palette.transform([](auto&& palette) { return palette.data(); })
                               .value_or(nullptr),
        });
}

float AppPageRenderer::calcFactor(
    Context& ctx,
    media::PageSize page_size,
    float scaling_coeff,
    const tui::TerminalDimensions& dims) {
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

ImageId AppPageRenderer::getIdForPage(const Page& page) const {
    auto [doc_id, page_index] = page;
    return image_id_salt_ + utils::cantorPair(doc_id, page_index + 1);
}

}  // namespace pdft::app
