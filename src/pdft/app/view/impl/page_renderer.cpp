#include "pdft/app/view/impl/page_renderer.h"

#include "pdft/core/result/combine/unwrap.h"
#include "pdft/core/result/syntax/pipe.h"

#include "pdft/utils/zlib.h"

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

void AppPageRenderer::render(
    Context& ctx,
    const Page& page,
    const PagePosition& pos,
    const DisplayProperties& orig_display,
    tui::Screen& screen) {
    const auto& [doc_id, page_index] = page;
    auto document = documents_->getDocument(page.document_id) | unwrap;
    auto the_page = ctx.push("get_page") | document->getPage(page_index);
    float base_factor = media::getBaseScalingFactor(
        the_page->getDefaultSize(), screen.size.width_px, screen.size.height_px);

    auto display = orig_display;
    display.scaling_coeff *= base_factor;
    auto id = getImageId(config_.image_id_salt, page, display);

    if (!state_.isLoaded(id)) {
        postPrepareImages(document, display, page);
        auto _ = ctx.push("load_image");
        loadImage(ctx, id, screen);
    }

    const auto& p = state_.getImage(id);
    float sub_rows = p.image_rows * pos.subarea.height;
    float sub_cols = p.image_cols * pos.subarea.width;
    auto irect = tui::fitImage(sub_cols / sub_rows, pos.rect, pos.placement);

    log::debug(ctx, "@={} I={}", pos.rect, irect);
    auto _ = ctx.push("display_image");
    display_->displayImage(ctx, id, irect, pos.subarea, screen.buffers);
}

void AppPageRenderer::postPrepareImages(
    Arc<const media::Document> document, const DisplayProperties& display, const Page& page) {
    const auto& [doc_id, page_index] = page;

    for (uint32_t i = 0; i < config_.num_preload_forward; ++i) {
        if (page_index + i >= document->getPageCount()) {
            return;
        }

        auto page = document->getPage(page_index + i);
        auto image_id = getImageId(config_.image_id_salt, {doc_id, page_index + i}, display);

        pool_.post(io_.get_executor(), image_id, [=, this](auto& ctx) {
            auto image = ctx.push("render_page") | loader_->renderPage(ctx, display, *page);
            auto compressed = ctx.push("compress") |
                              utils::zlibCompress(image::pixelsToSamples(image->samples()));

            return CompressedImage{
                .size = image->size(),
                .compressed_samples = std::move(compressed),
            };
        });
    }
}

void AppPageRenderer::loadImage(Context& ctx, tui::ImageId id, tui::Screen& screen) {
    auto&& [size, compressed_data] = ctx.push("await_task") | *pool_.get(id);

    {
        auto _ = ctx.push("load_image");
        display_->loadCompressedImage(
            ctx, id, size, compressed_data, screen.buffers.control_buffer);
    }

    float image_cols = float(size.width_px) / screen.size.cell_width_px;
    float image_rows = float(size.height_px) / screen.size.cell_height_px;
    state_.addImage(id, {.image_rows = image_rows, .image_cols = image_cols});
}

}  // namespace pdft::app
