#pragma once

#include "pdft/app/view/document_view.h"
#include "pdft/app/view/view.h"

namespace pdft::app::view::impl {

class View : public view::View {
 public:
    View(
        std::shared_ptr<const state::DocumentStorage> documents,
        std::shared_ptr<view::DocumentView> view)
        : documents_(std::move(documents))
        , doc_view_(std::move(view)) {}

    tui::Element render(
        const state::UIState& ui, tui::display::State& display, tui::Context& ctx) override;

 private:
    tui::Element getSidebar(const state::UIState& ui, tui::Context& ctx);

    std::shared_ptr<const state::DocumentStorage> documents_;
    std::shared_ptr<view::DocumentView> doc_view_;
};

}  // namespace pdft::app::view::impl
