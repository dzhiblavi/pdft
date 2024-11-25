#pragma once

#include "pdft/state/document_storage.h"
#include "pdft/tui/display/display.h"
#include "pdft/tui/terminal/terminal.h"
#include "pdft/view/renderer.h"

#include <unordered_map>

namespace pdft::view::app {

class Renderer : public view::Renderer {
 public:
    Renderer(
        std::shared_ptr<state::DocumentStorage> docs,
        std::shared_ptr<tui::display::Display> display);

    asio::awaitable<void> render(
        const tui::layout::IRect& rect,
        const tui::image::Placement& placement,
        const image::ImageProperties& properties,
        const Page& page,
        tui::Context& ctx) override;

 private:
    float calcFactor(
        media::PageSize page_size,
        float scaling_factor,
        const tui::terminal::Dimensions& screen_size);

    struct LoadedImage {
        media::PageSize page_size;
        bool inverted_colors;
        float factor;
        float cell_wh_ratio;
    };

    const uint32_t image_id_salt_;
    std::shared_ptr<state::DocumentStorage> docs_;
    std::shared_ptr<tui::display::Display> display_;
    std::shared_ptr<tui::terminal::Terminal> terminal_;
    std::unordered_map<image::ImageId, LoadedImage> loaded_;
};

}  // namespace pdft::view::app
