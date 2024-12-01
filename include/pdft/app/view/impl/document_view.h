#pragma once

#include "pdft/app/view/document_view.h"
#include "pdft/app/view/page_renderer.h"

namespace pdft::app::view::impl {

class DocumentView : public view::DocumentView {
 public:
    explicit DocumentView(std::shared_ptr<view::PageRenderer> renderer)
        : renderer_(std::move(renderer)) {}

    tui::Element render(
        const state::DocumentState& doc,
        const DisplayProperties& properties,
        tui::Context& ctx) override;

 private:
    tui::Element renderOnePage(
        const state::DocumentState& doc, const DisplayProperties& properties, tui::Context& ctx);
    tui::Element renderTwoPages(
        const state::DocumentState& doc, const DisplayProperties& properties, tui::Context& ctx);
    tui::Element renderOnePageContinuous(
        const state::DocumentState& doc, const DisplayProperties& properties, tui::Context& ctx);
    tui::Element renderTwoPagesContinuous(
        const state::DocumentState& doc, const DisplayProperties& properties, tui::Context& ctx);

    std::shared_ptr<view::PageRenderer> renderer_;
    tui::IRect rect_;
};

}  // namespace pdft::app::view::impl
