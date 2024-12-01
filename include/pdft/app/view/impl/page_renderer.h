#pragma once

#include "pdft/app/view/page_renderer.h"

#include "pdft/tui/display/display.h"

namespace pdft::app::view::impl {

class State {
 public:
    struct LoadedImage {
        float image_rows;
        float image_cols;
        float factor;
        bool invert_colors;

        // XXX Do not read from this pointer;
        // It should be used to determine whether the palette has been changed
        const image::RGBColor* palette_ptr = nullptr;
    };

    tui::display::ImageId addImage(tui::display::ImageId id, LoadedImage loaded);
    void removeImage(tui::display::ImageId id);

    bool isLoaded(tui::display::ImageId id) const;
    const LoadedImage& getImage(tui::display::ImageId id) const;

 private:
    std::unordered_map<tui::display::ImageId, LoadedImage> loaded_;
};

class PageRenderer : public view::PageRenderer {
 public:
    PageRenderer(
        uint32_t image_id_salt,
        std::shared_ptr<const state::DocumentStorage> documents_,
        std::shared_ptr<tui::display::Display> display);

    void render(
        const Page& page,
        const PagePosition& pos,
        const DisplayProperties& props,
        tui::Context& ctx) override;

 private:
    float calcFactor(media::PageSize page_size, float scaling_coeff, tui::Context& ctx);

    tui::display::ImageId getIdForPage(const Page& page) const;

    const uint32_t image_id_salt_;
    State state_;
    std::shared_ptr<const state::DocumentStorage> documents_;
    std::shared_ptr<tui::display::Display> display_;
};

}  // namespace pdft::app::view::impl
