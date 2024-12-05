#include "pdft/app/view/impl/page_renderer.h"

#include "pdft/core/result/combine/or_else.h"
#include "pdft/image/operations.h"
#include "pdft/image/palette.h"

#include "pdft/core/result/combine/and_then.h"
#include "pdft/core/result/combine/unwrap.h"
#include "pdft/core/result/syntax/pipe.h"

#include "pdft/utils/cantor_pair.h"

namespace pdft::app {

using tui::ImageId;
using namespace result;

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
    uint32_t image_id_salt,
    Arc<const DocumentStorage> documents,
    Arc<const PaletteStorage> palettes,
    Arc<tui::Display> display)
    : image_id_salt_(image_id_salt)
    , documents_(std::move(documents))
    , palettes_(std::move(palettes))
    , display_(std::move(display)) {}

void AppPageRenderer::render(
    Context& ctx,
    const Page& page,
    const PagePosition& pos,
    const DisplayProperties& display,
    tui::Screen& screen) {
    const auto& [doc_id, page_index] = page;

    auto document = documents_->getDocument(page.document_id) | unwrap;
    auto id = getIdForPage(page);
    auto the_page = ctx.push("get_page") | document->getPage(page_index);
    float base_factor = calcFactor(ctx, the_page->getDefaultSize(), screen.size);

    log::debug(ctx, "page: {}, image_id={}", page, id);

    if (state_.isLoaded(id) && shouldReload(id, base_factor, display)) {
        auto _ = ctx.push("unload_image");
        display_->unload(ctx, id, screen.buffers.control_buffer);
        state_.removeImage(id);
    }

    if (!state_.isLoaded(id)) {
        float factor = base_factor * display.scaling_coeff;
        auto image = ctx.push("get_image") | the_page->getImage(factor);
        ctx.push("load_image") | loadImage(ctx, id, *image, base_factor, display, screen);
    }

    const auto& p = state_.getImage(id);
    float sub_rows = p.image_rows * pos.subarea.height;
    float sub_cols = p.image_cols * pos.subarea.width;
    auto irect = tui::fitImage(sub_cols / sub_rows, pos.rect, pos.placement);

    log::debug(ctx, "@={} I={}", pos.rect, irect);
    auto _ = ctx.push("display_image");
    display_->displayImage(ctx, id, irect, pos.subarea, screen.buffers);
}

bool AppPageRenderer::shouldReload(
    tui::ImageId id, float base_factor, const DisplayProperties& display) {
    const auto& info = state_.getImage(id);

    if (base_factor != info.base_factor) {
        log::debug("loaded image base scaling factor has changed, unloading id={}", id);
        return true;
    }

    if (display != info.display) {
        log::debug("loaded image display settings have changed, unloading id={}", id);
        return true;
    }

    return false;
}

Unit AppPageRenderer::loadImage(
    Context& ctx,
    tui::ImageId id,
    image::Image& image,
    float base_factor,
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

        palettes_->get(*prop.palette)  //
            | andThen([&](image::PaletteRef palette) -> Maybe<Unit> {
                  image::applyPalette(image.mutableSamples(), palette);
                  return some();
              })  //
            | orElse([&]() -> Maybe<Unit> {
                  log::warn("failed to find a palette with name {}", *prop.palette);
                  return none;
              });
    }

    ctx.push("load_image") |
        (display_->loadImage(ctx, id, image, screen.buffers.control_buffer), unit);

    auto image_size = image.size();
    float image_cols = float(image_size.width_px) / screen.size.cell_width_px;
    float image_rows = float(image_size.height_px) / screen.size.cell_height_px;

    state_.addImage(
        id,
        {
            .image_rows = image_rows,
            .image_cols = image_cols,
            .base_factor = base_factor,
            .display = prop,
        });

    return unit;
}

float AppPageRenderer::calcFactor(
    Context& ctx, media::PageSize page_size, const tui::TerminalDimensions& dims) {
    float dw = dims.width_px;
    float dh = dims.height_px;
    auto [pw, ph] = page_size;

    float fw = dw / pw;
    float fh = dh / ph;
    float f = std::fmin(fw, fh);

    log::debug(
        ctx,
        "base factor for page {}x{}, term {}x{}, factor={}",
        page_size.width_px,
        page_size.height_px,
        dims.width_px,
        dims.height_px,
        f);

    return f;
}

ImageId AppPageRenderer::getIdForPage(const Page& page) const {
    auto [doc_id, page_index] = page;
    return image_id_salt_ + utils::cantorPair(doc_id, page_index + 1);
}

}  // namespace pdft::app
