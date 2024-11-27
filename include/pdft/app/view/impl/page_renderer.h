#pragma once

#include "pdft/app/state/document_storage.h"
#include "pdft/app/view/page_renderer.h"

#include "pdft/tui/display/display.h"
#include "pdft/tui/display/state.h"
#include "pdft/tui/terminal/terminal.h"

namespace pdft::app::view::impl {

class PageRenderer : public view::PageRenderer {
 public:
    PageRenderer(
        uint32_t image_id_salt,
        std::shared_ptr<state::DocumentStorage> docs,
        std::shared_ptr<tui::display::State> display_state,
        std::shared_ptr<tui::display::Display> display);

    void render(const Page& page, const PageProperties& properties, tui::Context& ctx) override;

 private:
    float calcFactor(media::PageSize page_size, float scaling_coeff, tui::Context& ctx);

    tui::display::ImageId getIdForPage(const Page& page) const;

    const uint32_t image_id_salt_;
    std::shared_ptr<state::DocumentStorage> docs_;
    std::shared_ptr<tui::display::State> display_state_;
    std::shared_ptr<tui::display::Display> display_;
    std::shared_ptr<tui::terminal::Terminal> terminal_;
};

}  // namespace pdft::app::view::impl
