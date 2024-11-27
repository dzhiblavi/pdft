#pragma once

#include "pdft/app/state/document_storage.h"
#include "pdft/app/view/document_view.h"
#include "pdft/app/view/view.h"

namespace pdft::app::view::impl {

class View : public view::View {
 public:
    View(std::shared_ptr<state::DocumentStorage> storage, std::shared_ptr<view::DocumentView> view)
        : doc_storage_(std::move(storage))
        , doc_view_(std::move(view)) {}

    tui::Element render(const state::UIState& state, tui::Context& ctx) override;

 private:
    tui::Element getSidebar(const state::UIState& state, tui::Context& ctx);

    std::shared_ptr<state::DocumentStorage> doc_storage_;
    std::shared_ptr<view::DocumentView> doc_view_;
};

}  // namespace pdft::app::view::impl
