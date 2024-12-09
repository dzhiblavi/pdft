#pragma once

#include "pdft/app/view/impl/image_renderer.h"
#include "pdft/app/view/page_renderer.h"
#include "pdft/tui/display.h"

#include "pdft/core/future.h"

#include <asio/io_context.hpp>
#include <asio/strand.hpp>

namespace pdft::app {

template <typename T>
struct FutureValue {
    explicit FutureValue(Future<T> future) : value_(std::move(future)) {}

    Async<const T*> get() {
        if (std::holds_alternative<Future<T>>(value_)) {
            Future<T> future = std::get<Future<T>>(std::move(value_));
            value_ = co_await std::move(future).async_extract(asio::use_awaitable);
        }

        co_return &std::get<T>(value_);
    }

 private:
    friend class BackgroundPool;

    std::variant<Future<T>, T> value_;
};

class AppPageRenderer : public PageRenderer {
 public:
    struct Config {
        uint32_t render_pool_size = 4;
        uint32_t num_preload_backward = 2;
        uint32_t num_preload_forward = 4;
        uint32_t image_id_salt;
    };

    AppPageRenderer(
        asio::io_context& io,
        Config config,
        Arc<const DocumentStorage> documents_,
        Arc<const ImageRenderer> renderer,
        Arc<tui::Display> display);

    ~AppPageRenderer();

    Async<Unit> render(
        Context& ctx,
        const Page& page,
        const PagePosition& pos,
        const DisplayProperties& props,
        tui::Screen& screen) override;

 private:
    struct LoadedImage {
        float image_rows;
        float image_cols;
    };

    struct RenderedImage {
        image::ImageSize size;
        std::string compressed_samples;
    };

    using RenderingImage = FutureValue<RenderedImage>;

    using ImageState = std::variant<LoadedImage, RenderingImage>;

    class Images {
     public:
        void add(tui::ImageId id, RenderingImage image);
        void remove(tui::ImageId id);

        bool contains(tui::ImageId id) const;
        ImageState& get(tui::ImageId id);

        bool isLoaded(tui::ImageId id);
        const LoadedImage& getLoadedImage(tui::ImageId id);
        RenderingImage& getRenderingImage(tui::ImageId id);

     private:
        std::unordered_map<tui::ImageId, ImageState> images_;
    };

    void postPrepareImages(
        Arc<const media::Document> document, const DisplayProperties& display, const Page& page);

    Async<void> loadImage(Context& ctx, tui::ImageId id, tui::Screen& screen);

    asio::io_context& io_;
    const Config config_;
    Images state_;
    Arc<const DocumentStorage> documents_;
    Arc<const ImageRenderer> renderer_;
    Arc<tui::Display> display_;
    asio::thread_pool pool_;
    asio::strand<asio::thread_pool::executor_type> media_strand_;
};

}  // namespace pdft::app
