#pragma once

#include "pdft/app/view/document_view.h"
#include "pdft/app/view/view.h"

namespace pdft::app::view::impl {

class View : public view::View {
 public:
    View(
        std::shared_ptr<const state::DocumentStorage> documents,
        std::shared_ptr<const view::DocumentView> view)
        : documents_(std::move(documents))
        , doc_view_(std::move(view)) {}

    tui::Element render(Context& ctx, const state::UIState& ui, tui::Screen& screen) override;

 private:
    tui::Element getSidebar(Context& ctx, const state::UIState& ui);

    std::shared_ptr<const state::DocumentStorage> documents_;
    std::shared_ptr<const view::DocumentView> doc_view_;
};

}  // namespace pdft::app::view::impl
