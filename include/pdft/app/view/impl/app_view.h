#pragma once

#include "pdft/app/state/ui.h"
#include "pdft/app/view/document_view.h"
#include "pdft/app/view/view.h"

#include "platform/module/module.h"

namespace pdft::app {

class AppView : public View, public platform::rt::Module {
 public:
    AppView(
        platform::Arc<const UI> ui,
        platform::Arc<const DocumentStorage> documents,
        platform::Arc<DocumentView> view);

    tui::Element getLayout(Context& ctx) override;
    platform::Async<void> render(Context& ctx, tui::Screen& screen) override;

 private:
    tui::Element getSidebar();
    platform::Maybe<tui::Element> getDocumentLayout(Context& ctx);

    platform::Arc<const UI> ui_;
    platform::Arc<const DocumentStorage> documents_;
    platform::Arc<DocumentView> doc_view_;
};

}  // namespace pdft::app
