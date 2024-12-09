#include "pdft/app/view/impl/page_renderer.h"

#include "pdft/core/result/combine/unwrap.h"
#include "pdft/core/result/syntax/pipe.h"

#include "pdft/utils/zlib.h"

namespace pdft::app {

using tui::ImageId;
using namespace result;

void AppPageRenderer::Images::add(tui::ImageId id, RenderingImage image) {
    PDFT_VERIFY(!contains(id));
    images_.emplace(id, std::move(image));
}

void AppPageRenderer::Images::remove(tui::ImageId id) {
    PDFT_VERIFY(contains(id));
    images_.erase(id);
}

bool AppPageRenderer::Images::contains(ImageId id) const {
    return images_.contains(id);
}

AppPageRenderer::ImageState& AppPageRenderer::Images::get(ImageId id) {
    PDFT_VERIFY(contains(id));
    return images_[id];
}

bool AppPageRenderer::Images::isLoaded(ImageId id) {
    return contains(id) && std::holds_alternative<LoadedImage>(get(id));
}

const AppPageRenderer::LoadedImage& AppPageRenderer::Images::getLoadedImage(tui::ImageId id) {
    return std::get<LoadedImage>(get(id));
}

AppPageRenderer::RenderingImage& AppPageRenderer::Images::getRenderingImage(tui::ImageId id) {
    return std::get<RenderingImage>(get(id));
}

AppPageRenderer::AppPageRenderer(
    asio::io_context& io,
    Config config,
    Arc<const DocumentStorage> documents,
    Arc<const PageLoader> loader,
    Arc<tui::Display> display)
    : io_(io)
    , config_(std::move(config))
    , documents_(std::move(documents))
    , loader_(std::move(loader))
    , display_(std::move(display))
    , pool_(1) {}

Async<Unit> AppPageRenderer::render(
    Context& ctx,
    const Page& page,
    const PagePosition& pos,
    const DisplayProperties& orig_display,
    tui::Screen& screen) {
    const auto& [doc_id, page_index] = page;
    auto document = documents_->getDocument(page.document_id) | unwrap;
    auto the_page = ctx.push("get_page") | document->getPage(page_index);
    float base_factor = media::getBaseScalingFactor(
        the_page->getDefaultSize(), screen.dimensions().width_px, screen.dimensions().height_px);

    auto display = orig_display;
    display.scaling_coeff *= base_factor;
    auto id = getImageId(config_.image_id_salt, page, display);

    if (!state_.isLoaded(id)) {
        postPrepareImages(document, display, page);
        auto _ = ctx.push("load_image");
        co_await loadImage(ctx, id, screen);
    }

    const auto& p = state_.getLoadedImage(id);
    float sub_rows = p.image_rows * pos.subarea.height;
    float sub_cols = p.image_cols * pos.subarea.width;
    auto irect = tui::fitImage(sub_cols / sub_rows, pos.rect, pos.placement);

    log::debug(ctx, "@={} I={}", pos.rect, irect);
    auto _ = ctx.push("display_image");
    display_->displayImage(ctx, id, irect, pos.subarea, screen.bufpair());

    co_return unit;
}

void AppPageRenderer::postPrepareImages(
    Arc<const media::Document> document, const DisplayProperties& display, const Page& page) {
    const auto& [doc_id, page_index] = page;

    for (uint32_t i = 0; i < config_.num_preload_forward; ++i) {
        if (page_index + i >= document->getPageCount()) {
            return;
        }

        auto image_id = getImageId(config_.image_id_salt, {doc_id, page_index + i}, display);

        if (state_.contains(image_id)) {
            log::info("skip image id {} because it is already rendered or loaded", image_id);
            continue;
        }

        auto page = document->getPage(page_index + i);

        auto future = pool_.post(io_.get_executor(), [=, this](auto& ctx) {
            auto image = ctx.push("render_page") | loader_->renderPage(ctx, display, *page);
            auto compressed = ctx.push("compress") |
                              utils::zlibCompress(image::pixelsToSamples(image->samples()));

            return RenderedImage{
                .size = image->size(),
                .compressed_samples = std::move(compressed),
            };
        });

        state_.add(image_id, std::move(future));
    }
}

Async<void> AppPageRenderer::loadImage(Context& ctx, tui::ImageId id, tui::Screen& screen) {
    auto&& image = state_.get(id);

    if (!std::holds_alternative<RenderingImage>(image)) {
        co_return;
    }

    auto&& rendering = std::get<RenderingImage>(image);
    auto&& [size, compressed_data] = ctx.push("await_task") | *(co_await rendering.get());

    {
        auto _ = ctx.push("load_image");
        display_->loadCompressedImage(
            ctx, id, size, compressed_data, screen.bufpair().control_buffer);
    }

    float image_cols = float(size.width_px) / screen.dimensions().cell_width_px;
    float image_rows = float(size.height_px) / screen.dimensions().cell_height_px;

    image = LoadedImage{.image_rows = image_rows, .image_cols = image_cols};
}

}  // namespace pdft::app
