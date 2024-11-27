#pragma once

#include "pdft/view/document_view.h"
#include "pdft/view/page_renderer.h"

#include "pdft/state/document_storage.h"

namespace pdft::view::app {

class DocumentView : public view::DocumentView {
 public:
    DocumentView(
        std::shared_ptr<state::DocumentStorage> doc_storage,
        std::shared_ptr<view::PageRenderer> renderer)
        : doc_storage_(std::move(doc_storage))
        , renderer_(std::move(renderer)) {}

    tui::Element render(
        const state::DocumentState& state,
        const image::ImageProperties& image,
        tui::Context& ctx) override;

 private:
    tui::Element renderOnePage(
        const state::DocumentState& state, const image::ImageProperties& image, tui::Context& ctx);
    tui::Element renderTwoPages(
        const state::DocumentState& state, const image::ImageProperties& image, tui::Context& ctx);
    tui::Element renderOnePageContinuous(
        const state::DocumentState& state, const image::ImageProperties& image, tui::Context& ctx);
    tui::Element renderTwoPagesContinuous(
        const state::DocumentState& state, const image::ImageProperties& image, tui::Context& ctx);

    std::shared_ptr<state::DocumentStorage> doc_storage_;
    std::shared_ptr<view::PageRenderer> renderer_;
    tui::IRect rect_;
};

}  // namespace pdft::view::app
