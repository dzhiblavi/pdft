#pragma once

#include "pdft/state/display_state.h"
#include "pdft/state/document_storage.h"
#include "pdft/tui/display/display.h"
#include "pdft/tui/terminal/terminal.h"
#include "pdft/view/page_renderer.h"

namespace pdft::view::app {

class PageRenderer : public view::PageRenderer {
 public:
    PageRenderer(
        uint32_t image_id_salt,
        std::shared_ptr<state::DisplayState> display_state,
        std::shared_ptr<state::DocumentStorage> docs,
        std::shared_ptr<tui::display::Display> display);

    void render(const Page& page, const PageProperties& properties, tui::Context& ctx) override;

 private:
    float calcFactor(
        media::PageSize page_size,
        float scaling_factor,
        const tui::terminal::Dimensions& screen_size,
        tui::Context& ctx);

    image::ImageId getIdForPage(const Page& page) const;

    const uint32_t image_id_salt_;
    std::shared_ptr<state::DisplayState> display_state_;
    std::shared_ptr<state::DocumentStorage> docs_;
    std::shared_ptr<tui::display::Display> display_;
    std::shared_ptr<tui::terminal::Terminal> terminal_;
};

}  // namespace pdft::view::app
