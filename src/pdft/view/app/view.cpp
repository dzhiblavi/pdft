#include "pdft/view/app/view.h"

namespace pdft::view::app {

tui::Element View::render(const state::State& state, tui::Context& ctx) {
    auto _ = ctx.push("state");

    auto doc = [&] {
        if (auto doc = state.getState()) {
            auto _ = ctx.push("render_doc");

            auto path = doc_storage_->getDocument(doc->document_id).getPath().filename();
            auto page_str = std::format("{}/{}", doc->page_index + 1, doc->page_count);

            return tui::window(
                tui::text(std::format("{} page {}", path.c_str(), page_str)),
                doc_view_->render(*doc, state.image_properties, ctx));
        }

        return tui::emptyElement();
    }();

    return tui::hbox({
        getSidebar(state, ctx),
        doc | tui::flex,
    });
}

tui::Element View::getSidebar(const state::State& state, tui::Context& ctx) {
    if (!state.show_sidebar) {
        return tui::emptyElement();
    }

    auto _ = ctx.push("render_sidebar");

    std::vector<tui::Element> elements;
    for (size_t i = 0; i < state.states.size(); ++i) {
        const auto& st = state.states[i];
        bool is_current = i == *state.idx;
        auto doc_id = st.document_id;
        const auto& path = doc_storage_->getDocument(doc_id).getPath();
        auto line = std::format("{}{}", is_current ? "* " : "", path.filename().c_str());
        elements.push_back(tui::text(std::move(line)));
    }

    return tui::vbox(std::move(elements))  //
           | tui::border                   //
           | tui::size(tui::WIDTH, tui::EQUAL, 20);
}

}  // namespace pdft::view::app
