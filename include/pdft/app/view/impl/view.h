#pragma once

#include "pdft/app/view/document_view.h"
#include "pdft/app/view/view.h"

namespace pdft::app {

class AppView : public View {
 public:
    AppView(Arc<const state::DocumentStorage> documents, Arc<const DocumentView> view)
        : documents_(std::move(documents))
        , doc_view_(std::move(view)) {}

    tui::Element render(Context& ctx, const state::UIState& ui, tui::Screen& screen) override;

 private:
    tui::Element getSidebar(Context& ctx, const state::UIState& ui);

    Arc<const state::DocumentStorage> documents_;
    Arc<const DocumentView> doc_view_;
};

}  // namespace pdft::app
