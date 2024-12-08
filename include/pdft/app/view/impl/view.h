#pragma once

#include "pdft/app/view/document_view.h"
#include "pdft/app/view/view.h"

namespace pdft::app {

class AppView : public View {
 public:
    AppView(Arc<const DocumentStorage> documents, Arc<const DocumentView> view)
        : documents_(std::move(documents))
        , doc_view_(std::move(view)) {}

    tui::Element render(Context& ctx, const UIState& ui, tui::Screen& screen) override;

 private:
    tui::Element getSidebar(const UIState& ui);
    Maybe<tui::Element> getDocument(Context& ctx, const UIState& ui, tui::Screen& screen);

    Arc<const DocumentStorage> documents_;
    Arc<const DocumentView> doc_view_;
};

}  // namespace pdft::app
