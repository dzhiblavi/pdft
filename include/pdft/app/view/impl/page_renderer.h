#pragma once

#include "pdft/app/view/page_renderer.h"

#include "pdft/tui/display/display.h"

namespace pdft::app::view::impl {

class PageRenderer : public view::PageRenderer {
 public:
    PageRenderer(
        uint32_t image_id_salt,
        std::shared_ptr<const state::DocumentStorage> documents_,
        std::shared_ptr<tui::display::Display> display);

    void render(
        const Page& page,
        const PageProperties& properties,
        tui::display::State& display,
        tui::Context& ctx) override;

 private:
    float calcFactor(media::PageSize page_size, float scaling_coeff, tui::Context& ctx);

    tui::display::ImageId getIdForPage(const Page& page) const;

    const uint32_t image_id_salt_;
    std::shared_ptr<const state::DocumentStorage> documents_;
    std::shared_ptr<tui::display::Display> display_;
};

}  // namespace pdft::app::view::impl
