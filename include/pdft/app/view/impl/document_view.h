#pragma once

#include "pdft/app/view/document_view.h"
#include "pdft/app/view/page_renderer.h"

#include "pdft/core/context.h"
#include "pdft/core/module/inject.h"
#include "pdft/core/module/module.h"

namespace pdft::app {

class AppDocumentView : public DocumentView, public Module {
 public:
    explicit AppDocumentView(inject<PageRenderer, "page_renderer"> renderer)
        : renderer_(std::move(renderer)) {}

    tui::Element getLayout(Context& ctx, const DocumentState& doc) override;

    Async<void> render(
        Context& ctx,
        const DocumentState& doc,
        const DisplayProperties& properties,
        tui::Screen& screen) const override;

 private:
    Async<void> renderOnePage(
        Context& ctx,
        const DocumentState& doc,
        const DisplayProperties& properties,
        tui::Screen& screen) const;
    Async<void> renderTwoPages(
        Context& ctx,
        const DocumentState& doc,
        const DisplayProperties& properties,
        tui::Screen& screen) const;
    Async<void> renderOnePageContinuous(
        Context& ctx,
        const DocumentState& doc,
        const DisplayProperties& properties,
        tui::Screen& screen) const;
    Async<void> renderTwoPagesContinuous(
        Context& ctx,
        const DocumentState& doc,
        const DisplayProperties& properties,
        tui::Screen& screen) const;

    tui::IRect tl, tr, bl, br, rect;
    Arc<PageRenderer> renderer_;
};

}  // namespace pdft::app
