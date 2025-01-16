#pragma once

#include "pdft/app/view/page_processor.h"
#include "pdft/app/view/page_renderer.h"
#include "pdft/tui/display.h"

#include "platform/core/future.h"
#include "platform/module/module.h"

#include "pdft/utils/lru_cache.h"
#include "pdft/utils/random.h"

#include <asio/io_context.hpp>
#include <asio/strand.hpp>
#include <asio/thread_pool.hpp>
#include <asio/use_awaitable.hpp>

namespace pdft::app {

template <typename T>
struct FutureValue {
    explicit FutureValue(platform::Future<T> future) : value_(std::move(future)) {}

    platform::Async<const T*> get() const {
        if (std::holds_alternative<platform::Future<T>>(value_)) {
            platform::Future<T> future = std::get<platform::Future<T>>(std::move(value_));
            value_ = co_await std::move(future).async_extract(asio::use_awaitable);
        }
        co_return &std::get<T>(value_);
    }

 private:
    friend class BackgroundPool;

    mutable std::variant<platform::Future<T>, T> value_;
};

class AppPageRenderer : public PageRenderer, public platform::rt::Module {
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
        platform::Arc<const DocumentStorage> documents,
        platform::Arc<PageProcessor> processor,
        platform::Arc<tui::Display> display);

    platform::Async<platform::Unit> render(
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

    platform::Async<void> renderImagesAround(
        platform::Arc<const media::Document> document,
        const DisplayProperties& display,
        const Page& page);

    platform::Async<void> loadImageToDisplay(Context& ctx, tui::ImageId id, tui::Screen& screen);

    const Config config_;
    platform::Arc<const DocumentStorage> documents_;
    platform::Arc<PageProcessor> processor_;
    platform::Arc<tui::Display> display_;

    utils::LRUCache<tui::ImageId, RenderingImage> rendered_;
    std::unordered_map<tui::ImageId, CellImageSize> sizes_;
};

}  // namespace pdft::app
