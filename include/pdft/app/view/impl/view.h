#pragma once

#include "pdft/app/view/document_view.h"
#include "pdft/app/view/view.h"

namespace pdft::app {

class AppView : public View {
 public:
    AppView(Arc<const DocumentStorage> documents, Arc<DocumentView> view)
        : documents_(std::move(documents))
        , doc_view_(std::move(view)) {}

    tui::Element getLayout(Context& ctx, const UIState& ui) override;
    Async<void> render(Context& ctx, const UIState& ui, tui::Screen& screen) override;

 private:
    tui::Element getSidebar(const UIState& ui);
    Maybe<tui::Element> getDocumentLayout(Context& ctx, const UIState& ui);

    Arc<const DocumentStorage> documents_;
    Arc<DocumentView> doc_view_;
};

}  // namespace pdft::app
