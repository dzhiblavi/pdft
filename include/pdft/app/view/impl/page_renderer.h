#pragma once

#include "pdft/app/view/impl/page_loader.h"
#include "pdft/app/view/page_renderer.h"
#include "pdft/tui/display.h"

#include "pdft/utils/background_pool.h"

#include <asio/io_context.hpp>

namespace pdft::app {

class AppPageRenderer : public PageRenderer {
 public:
    struct Config {
        uint32_t num_preload_forward = 4;
        uint32_t image_id_salt;
    };

    AppPageRenderer(
        asio::io_context& io,
        Config config,
        Arc<const DocumentStorage> documents_,
        Arc<const PageLoader> loader,
        Arc<tui::Display> display);

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

    struct CompressedImage {
        image::ImageSize size;
        std::string compressed_samples;
    };

    class State {
     public:
        tui::ImageId addImage(tui::ImageId id, LoadedImage loaded);
        void removeImage(tui::ImageId id);

        bool isLoaded(tui::ImageId id) const;
        const LoadedImage& getImage(tui::ImageId id) const;

     private:
        std::unordered_map<tui::ImageId, LoadedImage> loaded_;
    };

    void postPrepareImages(
        Arc<const media::Document> document, const DisplayProperties& display, const Page& page);

    Async<void> loadImage(Context& ctx, tui::ImageId id, tui::Screen& screen);

    asio::io_context& io_;
    const Config config_;
    State state_;
    Arc<const DocumentStorage> documents_;
    Arc<const PageLoader> loader_;
    Arc<tui::Display> display_;
    utils::BackgroundPool<tui::ImageId, CompressedImage> pool_;
};

}  // namespace pdft::app
