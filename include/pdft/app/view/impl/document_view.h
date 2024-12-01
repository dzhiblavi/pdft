#pragma once

#include "pdft/app/view/document_view.h"
#include "pdft/app/view/page_renderer.h"
#include "pdft/core/context.h"

namespace pdft::app::view::impl {

class DocumentView : public view::DocumentView {
 public:
    explicit DocumentView(std::shared_ptr<view::PageRenderer> renderer)
        : renderer_(std::move(renderer)) {}

    tui::Element render(
        Context& ctx,
        const state::DocumentState& doc,
        const DisplayProperties& properties,
        tui::Screen& screen) const override;

 private:
    tui::Element renderOnePage(
        Context& ctx,
        const state::DocumentState& doc,
        const DisplayProperties& properties,
        tui::Screen& screen) const;
    tui::Element renderTwoPages(
        Context& ctx,
        const state::DocumentState& doc,
        const DisplayProperties& properties,
        tui::Screen& screen) const;
    tui::Element renderOnePageContinuous(
        Context& ctx,
        const state::DocumentState& doc,
        const DisplayProperties& properties,
        tui::Screen& screen) const;
    tui::Element renderTwoPagesContinuous(
        Context& ctx,
        const state::DocumentState& doc,
        const DisplayProperties& properties,
        tui::Screen& screen) const;

    std::shared_ptr<view::PageRenderer> renderer_;
    mutable tui::IRect rect_;
};

}  // namespace pdft::app::view::impl
