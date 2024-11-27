#pragma once

#include "pdft/app/state/document_storage.h"
#include "pdft/app/view/document_view.h"
#include "pdft/app/view/page_renderer.h"

namespace pdft::app::view::impl {

class DocumentView : public view::DocumentView {
 public:
    DocumentView(
        std::shared_ptr<state::DocumentStorage> doc_storage,
        std::shared_ptr<view::PageRenderer> renderer)
        : doc_storage_(std::move(doc_storage))
        , renderer_(std::move(renderer)) {}

    tui::Element render(
        const state::DocumentState& state,
        float scaling_coeff,
        bool invert_colors,
        tui::Context& ctx) override;

 private:
    tui::Element renderOnePage(
        const state::DocumentState& state,
        float scaling_coeff,
        bool invert_colors,
        tui::Context& ctx);
    tui::Element renderTwoPages(
        const state::DocumentState& state,
        float scaling_coeff,
        bool invert_colors,
        tui::Context& ctx);
    tui::Element renderOnePageContinuous(
        const state::DocumentState& state,
        float scaling_coeff,
        bool invert_colors,
        tui::Context& ctx);
    tui::Element renderTwoPagesContinuous(
        const state::DocumentState& state,
        float scaling_coeff,
        bool invert_colors,
        tui::Context& ctx);

    std::shared_ptr<state::DocumentStorage> doc_storage_;
    std::shared_ptr<view::PageRenderer> renderer_;
    tui::IRect rect_;
};

}  // namespace pdft::app::view::impl
