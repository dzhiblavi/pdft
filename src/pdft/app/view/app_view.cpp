#include "pdft/app/view/impl/app_view.h"

#include "platform/module/register.h"

#include "platform/core/result.h"

namespace pdft::app {

using namespace result;
using namespace platform;

AppView::AppView(
    platform::Arc<const UI> ui,
    platform::Arc<const DocumentStorage> documents,
    platform::Arc<DocumentView> view)
    : ui_(std::move(ui))
    , documents_(std::move(documents))
    , doc_view_(std::move(view)) {}

tui::Element AppView::getLayout(Context& ctx) {
    auto doc = getDocumentLayout(ctx).value_or(tui::emptyElement());

    if (!ui_->getModel().sidebar) {
        return doc;
    }

    return tui::hbox({
        ctx.push("appview:sidebar") | getSidebar(),
        doc | tui::flex,
    });
}

platform::Async<void> AppView::render(Context& ctx, tui::Screen& screen) {
    if (auto state = ui_->getDocument()) {
        co_await doc_view_->render(ctx, *state, ui_->getModel().display, screen);
    }
}

platform::Maybe<tui::Element> AppView::getDocumentLayout(Context& ctx) {
    if (!ui_->getDocument()) {
        return none;
    }
    auto state = ui_->getDocument();
    auto document = documents_->getDocument(state->document_id) | unwrap;
    auto path = document->getPath().filename();
    auto page_str = std::format("{}/{}", state->page_index + 1, state->page_count);

    return tui::window(
        tui::text(std::format("{} page {}", path.c_str(), page_str)),
        ctx.push("appview:document") | doc_view_->getLayout(ctx, *state));
}

tui::Element AppView::getSidebar() {
    static constexpr int side_bar_width = 20;

    const UIModel& ui = ui_->getModel();
    std::vector<tui::Element> elements;
    elements.reserve(ui.documents.size());

    for (size_t i = 0; i < ui.documents.size(); ++i) {
        const auto& st = ui.documents[i];
        bool is_current = i == *ui.index;
        auto doc_id = st.document_id;

        auto document = documents_->getDocument(doc_id) | unwrap;
        const auto& path = document->getPath();
        auto line = std::format("{}{}", is_current ? "* " : "", path.filename().c_str());
        elements.push_back(tui::text(std::move(line)));
    }

    return tui::vbox(std::move(elements))  //
           | tui::border                   //
           | tui::size(tui::WIDTH, tui::EQUAL, side_bar_width);
}

}  // namespace pdft::app

PLATFORM_REGISTER_MODULE(pdft::app::AppView);
