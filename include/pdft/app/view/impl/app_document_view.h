#pragma once

#include "pdft/app/view/document_view.h"
#include "pdft/app/view/page_renderer.h"

#include "platform/module/module.h"
#include "platform/trace/context.h"

namespace pdft::app {

class AppDocumentView : public DocumentView, public platform::rt::Module {
 public:
    explicit AppDocumentView(platform::Arc<PageRenderer> renderer)
        : renderer_(std::move(renderer)) {}

    tui::Element getLayout(platform::trace::Context& ctx, const DocumentModel& doc) override;

    platform::Async<void> render(
        platform::trace::Context& ctx,
        const DocumentModel& doc,
        const DisplayProperties& properties,
        tui::Screen& screen) const override;

 private:
    platform::Async<void> renderOnePage(
        platform::trace::Context& ctx,
        const DocumentModel& doc,
        const DisplayProperties& properties,
        tui::Screen& screen) const;
    platform::Async<void> renderTwoPages(
        platform::trace::Context& ctx,
        const DocumentModel& doc,
        const DisplayProperties& properties,
        tui::Screen& screen) const;
    platform::Async<void> renderOnePageContinuous(
        platform::trace::Context& ctx,
        const DocumentModel& doc,
        const DisplayProperties& properties,
        tui::Screen& screen) const;
    platform::Async<void> renderTwoPagesContinuous(
        platform::trace::Context& ctx,
        const DocumentModel& doc,
        const DisplayProperties& properties,
        tui::Screen& screen) const;

    tui::IRect tl, tr, bl, br, rect;
    platform::Arc<PageRenderer> renderer_;
};

}  // namespace pdft::app
