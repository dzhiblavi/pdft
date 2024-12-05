#pragma once

#include "pdft/app/view/page_renderer.h"

#include "pdft/tui/display.h"
#include "pdft/tui/terminal.h"

namespace pdft::app {

class AppPageRenderer : public PageRenderer {
 public:
    AppPageRenderer(
        uint32_t image_id_salt, Arc<const DocumentStorage> documents_, Arc<tui::Display> display);

    void render(
        Context& ctx,
        const Page& page,
        const PagePosition& pos,
        const DisplayProperties& props,
        tui::Screen& screen) override;

 private:
    struct LoadedImage {
        float image_rows;
        float image_cols;
        float factor;
        bool invert_colors;
        bool make_bolder;

        // XXX Do not read from this pointer;
        // It should be used to determine whether the palette has been changed
        const image::RGBColor* palette_ptr = nullptr;
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

    float calcFactor(
        Context& ctx,
        media::PageSize page_size,
        float scaling_coeff,
        const tui::TerminalDimensions& dims);

    bool shouldReload(tui::ImageId id, float factor, const DisplayProperties& prop);

    void loadImage(
        Context& ctx,
        tui::ImageId id,
        image::Image& image,
        float factor,
        const DisplayProperties& prop,
        tui::Screen& screen);

    tui::ImageId getIdForPage(const Page& page) const;

    const uint32_t image_id_salt_;
    State state_;
    Arc<const DocumentStorage> documents_;
    Arc<tui::Display> display_;
};

}  // namespace pdft::app
