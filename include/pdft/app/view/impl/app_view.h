#pragma once

#include "pdft/app/state/ui.h"
#include "pdft/app/view/document_view.h"
#include "pdft/app/view/view.h"

#include "pdft/runtime/module/module.h"

namespace pdft::app {

class AppView : public View, public rt::Module {
 public:
    AppView(Arc<const UI> ui, Arc<const DocumentStorage> documents, Arc<DocumentView> view);

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
