#pragma once

#include "pdft/app/view/document_view.h"
#include "pdft/app/view/page_renderer.h"

#include "pdft/core/context.h"
#include "platform/module/module.h"

namespace pdft::app {

class AppDocumentView : public DocumentView, public platform::rt::Module {
 public:
    explicit AppDocumentView(platform::Arc<PageRenderer> renderer) : renderer_(std::move(renderer)) {}

    tui::Element getLayout(Context& ctx, const DocumentModel& doc) override;

    platform::Async<void> render(
        Context& ctx,
        const DocumentModel& doc,
        const DisplayProperties& properties,
        tui::Screen& screen) const override;

 private:
    platform::Async<void> renderOnePage(
        Context& ctx,
        const DocumentModel& doc,
        const DisplayProperties& properties,
        tui::Screen& screen) const;
    platform::Async<void> renderTwoPages(
        Context& ctx,
        const DocumentModel& doc,
        const DisplayProperties& properties,
        tui::Screen& screen) const;
    platform::Async<void> renderOnePageContinuous(
        Context& ctx,
        const DocumentModel& doc,
        const DisplayProperties& properties,
        tui::Screen& screen) const;
    platform::Async<void> renderTwoPagesContinuous(
        Context& ctx,
        const DocumentModel& doc,
        const DisplayProperties& properties,
        tui::Screen& screen) const;

    tui::IRect tl, tr, bl, br, rect;
    platform::Arc<PageRenderer> renderer_;
};

}  // namespace pdft::app
