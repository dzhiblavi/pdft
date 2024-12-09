#include "pdft/app/view/impl/view.h"

#include "pdft/core/result/combine/unwrap.h"
#include "pdft/core/result/coro/maybe.h"
#include "pdft/core/result/make/from_ptr.h"
#include "pdft/core/result/syntax/pipe.h"

namespace pdft::app {

using namespace result;

tui::Element AppView::getLayout(Context& ctx, const UIState& ui) {
    auto doc = getDocumentLayout(ctx, ui).value_or(tui::emptyElement());

    if (!ui.show_sidebar) {
        return doc;
    }

    return tui::hbox({
        ctx.push("appview:sidebar") | getSidebar(ui),
        doc | tui::flex,
    });
}

Async<void> AppView::render(Context& ctx, const UIState& ui, tui::Screen& screen) {
    if (auto state = ui.getImmutableState()) {
        co_await doc_view_->render(ctx, *state, ui.display, screen);
    }
}

Maybe<tui::Element> AppView::getDocumentLayout(Context& ctx, const UIState& ui) {
    auto state = co_await fromPtr(ui.getImmutableState());
    auto document = documents_->getDocument(state->document_id) | unwrap;
    auto path = document->getPath().filename();
    auto page_str = std::format("{}/{}", state->page_index + 1, state->page_count);

    co_return tui::window(
        tui::text(std::format("{} page {}", path.c_str(), page_str)),
        ctx.push("appview:document") | doc_view_->getLayout(ctx, *state));
}

tui::Element AppView::getSidebar(const UIState& ui) {
    std::vector<tui::Element> elements;
    elements.reserve(ui.states.size());

    for (size_t i = 0; i < ui.states.size(); ++i) {
        const auto& st = ui.states[i];
        bool is_current = i == *ui.idx;
        auto doc_id = st.document_id;

        auto document = documents_->getDocument(doc_id) | unwrap;
        const auto& path = document->getPath();
        auto line = std::format("{}{}", is_current ? "* " : "", path.filename().c_str());
        elements.push_back(tui::text(std::move(line)));
    }

    return tui::vbox(std::move(elements))  //
           | tui::border                   //
           | tui::size(tui::WIDTH, tui::EQUAL, 20);
}

}  // namespace pdft::app
