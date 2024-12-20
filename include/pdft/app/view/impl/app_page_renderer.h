#pragma once

#include "pdft/app/view/page_processor.h"
#include "pdft/app/view/page_renderer.h"
#include "pdft/tui/display.h"

#include "pdft/core/future.h"
#include "pdft/runtime/module/module.h"

#include "pdft/utils/lru_cache.h"
#include "pdft/utils/random.h"

#include <asio/io_context.hpp>
#include <asio/strand.hpp>
#include <asio/thread_pool.hpp>

namespace pdft::app {

template <typename T>
struct FutureValue {
    explicit FutureValue(Future<T> future) : value_(std::move(future)) {}

    Async<const T*> get() const {
        if (std::holds_alternative<Future<T>>(value_)) {
            Future<T> future = std::get<Future<T>>(std::move(value_));
            value_ = co_await std::move(future).async_extract(asio::use_awaitable);
        }
        co_return &std::get<T>(value_);
    }

 private:
    friend class BackgroundPool;

    mutable std::variant<Future<T>, T> value_;
};

class AppPageRenderer : public PageRenderer, public rt::Module {
 public:
    struct Config {
        static constexpr uint32_t DefaultImageCacheSize = 128;

        uint32_t image_cache_size = DefaultImageCacheSize;
        uint32_t num_preload_backward = 2;
        uint32_t num_preload_forward = 4;
        uint32_t image_id_salt = utils::random<uint32_t>();
    };

    AppPageRenderer(
        Config config,
        Arc<const DocumentStorage> documents,
        Arc<PageProcessor> processor,
        Arc<tui::Display> display);

    Async<Unit> render(
        Context& ctx,
        const Page& page,
        const PagePosition& pos,
        const DisplayProperties& props,
        tui::Screen& screen) override;

 private:
    struct CellImageSize {
        float image_rows;
        float image_cols;
    };

    using RenderingImage = FutureValue<ProcessedImage>;

    Async<void> renderImagesAround(
        Arc<const media::Document> document, const DisplayProperties& display, const Page& page);

    Async<void> loadImageToDisplay(Context& ctx, tui::ImageId id, tui::Screen& screen);

    const Config config_;
    Arc<const DocumentStorage> documents_;
    Arc<PageProcessor> processor_;
    Arc<tui::Display> display_;

    utils::LRUCache<tui::ImageId, RenderingImage> rendered_;
    std::unordered_map<tui::ImageId, CellImageSize> sizes_;
};

}  // namespace pdft::app
