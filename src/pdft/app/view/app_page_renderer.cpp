#include "pdft/app/view/impl/app_page_renderer.h"

#include "platform/module/register.h"

#include "platform/core/result.h"

#include <asio/bind_executor.hpp>
#include <asio/co_spawn.hpp>
#include <asio/detached.hpp>

namespace pdft::app {

using tui::ImageId;
using namespace result;
using namespace platform;

AppPageRenderer::AppPageRenderer(
    Config config,
    platform::Arc<const DocumentStorage> documents,
    platform::Arc<PageProcessor> processor,
    platform::Arc<tui::Display> display)
    : config_(std::move(config))
    , documents_(std::move(documents))
    , processor_(std::move(processor))
    , display_(std::move(display))
    , rendered_(config.image_cache_size) {}

platform::Async<platform::Unit> AppPageRenderer::render(
    platform::trace::Context& ctx,
    const Page& page,
    const PagePosition& pos,
    const DisplayProperties& orig_display,
    tui::Screen& screen) {
    const auto& [doc_id, page_index] = page;
    auto document = documents_->getDocument(page.document_id) | unwrap;
    auto the_page = ctx.push("get_page") | document->getPage(int(page_index));
    float base_factor = media::getBaseScalingFactor(
        the_page->getDefaultSize(),
        float(screen.dimensions().width_px),
        float(screen.dimensions().height_px));

    auto display = orig_display;
    display.scaling_coeff *= base_factor;
    auto id = getImageId(config_.image_id_salt, page, display);

    co_await renderImagesAround(document, display, page);

    if (!display_->isLoaded(id)) {
        co_await loadImageToDisplay(ctx, id, screen);
    }

    auto [rows, cols] = sizes_[id];
    float sub_rows = rows * pos.subarea.height;
    float sub_cols = cols * pos.subarea.width;
    auto irect = tui::fitImage(sub_cols / sub_rows, pos.rect, pos.placement);

    log::debug(this, "@={} I={}", pos.rect, irect);
    auto _ = ctx.push("display_image");
    display_->displayImage(ctx, id, irect, pos.subarea, screen.bufpair());

    co_return unit;
}

platform::Async<void> AppPageRenderer::renderImagesAround(
    platform::Arc<const media::Document> document,
    const DisplayProperties& display,
    const Page& page) {
    const auto& [doc_id, page_index] = page;

    int forward = int(config_.num_preload_forward);
    int backward = int(config_.num_preload_backward);
    int page_count = int(document->getPageCount());

    for (int i = -backward; i < forward; ++i) {
        int index = i + static_cast<int>(page_index);

        if (index < 0 || index >= page_count) {
            continue;
        }

        uint32_t page_index = static_cast<uint32_t>(index);
        auto image_id = getImageId(config_.image_id_salt, {doc_id, page_index}, display);

        if (rendered_.contains(image_id)) {
            continue;
        }

        log::info("(background) rendering image with id={}", image_id);
        auto [future, promise] = co_await makeContract<ProcessedImage>();
        processor_->process(document->getPage(int(page_index)), display, std::move(promise));

        if (auto unloaded = rendered_.add(image_id, FutureValue{std::move(future)})) {
            auto&& [id, _] = *unloaded;
            log::debug("unloaded rendered image with id {}", id);
        }
    }
}

platform::Async<void> AppPageRenderer::loadImageToDisplay(
    platform::trace::Context& ctx, tui::ImageId id, tui::Screen& screen) {
    auto* future = rendered_.tryGet(id);
    if (!future) {
        panic("image with id {} is not rendered", id);
    }

    auto&& [size, compressed_data] = ctx.push("await_task") | *(co_await future->get());
    {
        auto _ = ctx.push("load_image");
        display_->loadCompressedImage(
            ctx, id, size, compressed_data, screen.bufpair().control_buffer);
    }

    float image_cols = float(size.width_px) / float(screen.dimensions().cell_width_px);
    float image_rows = float(size.height_px) / float(screen.dimensions().cell_height_px);

    sizes_[id] = CellImageSize{
        .image_rows = image_rows,
        .image_cols = image_cols,
    };
}

}  // namespace pdft::app

PLATFORM_REGISTER_MODULE(pdft::app::AppPageRenderer);
