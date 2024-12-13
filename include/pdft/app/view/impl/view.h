#pragma once

#include "pdft/app/state/ui.h"
#include "pdft/app/view/document_view.h"
#include "pdft/app/view/view.h"

#include "pdft/core/module/inject.h"
#include "pdft/core/module/module.h"

namespace pdft::app {

class AppView : public View, public Module {
 public:
    AppView(
        inject<const UI, "ui"> ui,
        inject<const DocumentStorage, "docs"> documents,
        inject<DocumentView, "doc_view"> view);

    tui::Element getLayout(Context& ctx) override;
    Async<void> render(Context& ctx, tui::Screen& screen) override;

 private:
    tui::Element getSidebar();
    Maybe<tui::Element> getDocumentLayout(Context& ctx);

    Arc<const UI> ui_;
    Arc<const DocumentStorage> documents_;
    Arc<DocumentView> doc_view_;
};

}  // namespace pdft::app
