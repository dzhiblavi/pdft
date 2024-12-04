#pragma once

#include "pdft/app/view/document_view.h"
#include "pdft/app/view/page_renderer.h"
#include "pdft/core/context.h"

namespace pdft::app {

class AppDocumentView : public DocumentView {
 public:
    explicit AppDocumentView(Arc<PageRenderer> renderer) : renderer_(std::move(renderer)) {}

    tui::Element render(
        Context& ctx,
        const DocumentState& doc,
        const DisplayProperties& properties,
        tui::Screen& screen) const override;

 private:
    tui::Element renderOnePage(
        Context& ctx,
        const DocumentState& doc,
        const DisplayProperties& properties,
        tui::Screen& screen) const;
    tui::Element renderTwoPages(
        Context& ctx,
        const DocumentState& doc,
        const DisplayProperties& properties,
        tui::Screen& screen) const;
    tui::Element renderOnePageContinuous(
        Context& ctx,
        const DocumentState& doc,
        const DisplayProperties& properties,
        tui::Screen& screen) const;
    tui::Element renderTwoPagesContinuous(
        Context& ctx,
        const DocumentState& doc,
        const DisplayProperties& properties,
        tui::Screen& screen) const;

    Arc<PageRenderer> renderer_;
    mutable tui::IRect rect_;
};

}  // namespace pdft::app
