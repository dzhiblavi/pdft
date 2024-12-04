#include "pdft/app/view/impl/view.h"

#include "pdft/core/result/combine/unwrap.h"
#include "pdft/core/result/syntax/pipe.h"

namespace pdft::app {

tui::Element AppView::render(Context& ctx, const UIState& ui, tui::Screen& screen) {
    auto _ = ctx.push("state");

    auto doc = [&] {
        if (auto doc = ui.getImmutableState()) {
            auto _ = ctx.push("render_doc");

            auto document = documents_->getDocument(doc->document_id) | result::unwrap;
            auto path = document->getPath().filename();
            auto page_str = std::format("{}/{}", doc->page_index + 1, doc->page_count);

            auto display_properties = DisplayProperties{
                .scaling_coeff = ui.scaling_coeff,
                .invert_colors = ui.invert_colors,
                .make_bolder = ui.make_bolder,
                .palette = ui.palette_name.transform([&](auto&& name) {
                    return image::getBuiltinPalettes().at(name);
                }),
            };

            return tui::window(
                tui::text(std::format("{} page {}", path.c_str(), page_str)),
                doc_view_->render(ctx, *doc, display_properties, screen));
        }

        return tui::emptyElement();
    }();

    return tui::hbox({
        getSidebar(ctx, ui),
        doc | tui::flex,
    });
}

tui::Element AppView::getSidebar(Context& ctx, const UIState& ui) {
    if (!ui.show_sidebar) {
        return tui::emptyElement();
    }

    auto _ = ctx.push("render_sidebar");

    std::vector<tui::Element> elements;
    for (size_t i = 0; i < ui.states.size(); ++i) {
        const auto& st = ui.states[i];
        bool is_current = i == *ui.idx;
        auto doc_id = st.document_id;

        auto document = documents_->getDocument(doc_id) | result::unwrap;
        const auto& path = document->getPath();
        auto line = std::format("{}{}", is_current ? "* " : "", path.filename().c_str());
        elements.push_back(tui::text(std::move(line)));
    }

    return tui::vbox(std::move(elements))  //
           | tui::border                   //
           | tui::size(tui::WIDTH, tui::EQUAL, 20);
}

}  // namespace pdft::app
