#include "pdft/app/view/impl/document_view.h"

namespace pdft::app {

tui::Element AppDocumentView::render(
    Context& ctx,
    const DocumentState& doc,
    const DisplayProperties& properties,
    tui::Screen& screen) const {
    auto _ = ctx.push("render");

    switch (doc.view_mode) {
        using enum ViewMode;

        case OnePage:
            return renderOnePage(ctx, doc, properties, screen);

        case TwoPages:
            return renderTwoPages(ctx, doc, properties, screen);

        case OnePageContinuous:
            return renderOnePageContinuous(ctx, doc, properties, screen);

        case TwoPagesContinuous:
            return renderTwoPagesContinuous(ctx, doc, properties, screen);
    }
}

tui::Element AppDocumentView::renderOnePage(
    Context& ctx,
    const DocumentState& doc,
    const DisplayProperties& properties,
    tui::Screen& screen) const {
    auto render = [&, properties](tui::IRect rect) {
        auto _ = ctx.push("only_page");

        renderer_->render(
            ctx,
            {
                .document_id = doc.document_id,
                .page_index = doc.page_index,
            },
            {
                .rect = rect,
                .placement =
                    {
                        .hor = tui::Alignment::Center,
                        .vert = tui::Alignment::Center,
                    },
            },
            properties,
            screen);
    };

    return tui::withSize(render);
}

tui::Element AppDocumentView::renderTwoPages(
    Context& ctx,
    const DocumentState& doc,
    const DisplayProperties& properties,
    tui::Screen& screen) const {
    auto render_left = [&, properties](tui::IRect rect) {
        auto _ = ctx.push("left_page");

        renderer_->render(
            ctx,
            {
                .document_id = doc.document_id,
                .page_index = doc.page_index,
            },
            {
                .rect = rect,
                .placement =
                    {
                        .hor = tui::Alignment::Max,
                        .vert = tui::Alignment::Center,
                    },
            },
            properties,
            screen);
    };

    auto render_right = [&, properties](tui::IRect rect) {
        if (doc.page_index + 1 >= doc.page_count) {
            return;
        }

        auto _ = ctx.push("right_page");

        renderer_->render(
            ctx,
            {
                .document_id = doc.document_id,
                .page_index = doc.page_index + 1,
            },
            {
                .rect = rect,
                .placement =
                    {
                        .hor = tui::Alignment::Min,
                        .vert = tui::Alignment::Center,
                    },
            },
            properties,
            screen);
    };

    return tui::hbox({
        tui::withSize(render_left) | tui::flex,
        tui::withSize(render_right) | tui::flex,
    });
}

tui::Element AppDocumentView::renderOnePageContinuous(
    Context& ctx,
    const DocumentState& doc,
    const DisplayProperties& properties,
    tui::Screen& screen) const {
    auto render_top = [&, properties](tui::IRect rect) {
        auto _ = ctx.push("top_page");
        float this_ratio = float(rect.height) / rect_.height;

        renderer_->render(
            ctx,
            {
                .document_id = doc.document_id,
                .page_index = doc.page_index,
            },
            {
                .rect = rect,
                .subarea =
                    {
                        .x = 0.f,
                        .y = 1.f - this_ratio,
                        .width = 1.f,
                        .height = this_ratio,
                    },
                .placement =
                    {
                        .hor = tui::Alignment::Center,
                        .vert = tui::Alignment::Max,
                    },
            },
            properties,
            screen);
    };

    auto render_bottom = [&, properties](tui::IRect rect) {
        if (doc.page_index + 1 >= doc.page_count) {
            return;
        }
        auto _ = ctx.push("bottom_page");
        float this_ratio = float(rect.height) / rect_.height;

        renderer_->render(
            ctx,
            {
                .document_id = doc.document_id,
                .page_index = doc.page_index + 1,
            },
            {
                .rect = rect,
                .subarea =
                    {
                        .x = 0.f,
                        .y = 0.f,
                        .width = 1.f,
                        .height = this_ratio,
                    },
                .placement =
                    {
                        .hor = tui::Alignment::Center,
                        .vert = tui::Alignment::Min,
                    },
            },
            properties,
            screen);
    };

    return tui::vbox({
               tui::withSize(render_top) | tui::flex |
                   tui::size(tui::HEIGHT, tui::EQUAL, 100 - doc.scroll_offset),
               tui::withSize(render_bottom) | tui::flex |
                   tui::size(tui::HEIGHT, tui::EQUAL, doc.scroll_offset),
           }) |
           tui::reflect(rect_);
}

tui::Element AppDocumentView::renderTwoPagesContinuous(
    Context& ctx,
    const DocumentState& doc,
    const DisplayProperties& properties,
    tui::Screen& screen) const {
    auto render_top_left = [&, properties](tui::IRect rect) {
        auto _ = ctx.push("top_left_page");
        float this_ratio = float(rect.height) / rect_.height;

        renderer_->render(
            ctx,
            {
                .document_id = doc.document_id,
                .page_index = doc.page_index,
            },
            {
                .rect = rect,
                .subarea =
                    {
                        .x = 0.f,
                        .y = 1.f - this_ratio,
                        .width = 1.f,
                        .height = this_ratio,
                    },
                .placement =
                    {
                        .hor = tui::Alignment::Max,
                        .vert = tui::Alignment::Max,
                    },
            },
            properties,
            screen);
    };

    auto render_top_right = [&, properties](tui::IRect rect) {
        if (doc.page_index + 1 >= doc.page_count) {
            return;
        }

        auto _ = ctx.push("top_right_page");
        float this_ratio = float(rect.height) / rect_.height;

        renderer_->render(
            ctx,
            {
                .document_id = doc.document_id,
                .page_index = doc.page_index + 1,
            },
            {
                .rect = rect,
                .subarea =
                    {
                        .x = 0.f,
                        .y = 1.f - this_ratio,
                        .width = 1.f,
                        .height = this_ratio,
                    },
                .placement =
                    {
                        .hor = tui::Alignment::Min,
                        .vert = tui::Alignment::Max,
                    },
            },
            properties,
            screen);
    };

    auto render_bottom_left = [&, properties](tui::IRect rect) {
        if (doc.page_index + 2 >= doc.page_count) {
            return;
        }

        auto _ = ctx.push("bottom_left_page");
        float this_ratio = float(rect.height) / rect_.height;

        renderer_->render(
            ctx,
            {
                .document_id = doc.document_id,
                .page_index = doc.page_index + 2,
            },
            {
                .rect = rect,
                .subarea =
                    {
                        .x = 0.f,
                        .y = 0.f,
                        .width = 1.f,
                        .height = this_ratio,
                    },
                .placement =
                    {
                        .hor = tui::Alignment::Max,
                        .vert = tui::Alignment::Min,
                    },
            },
            properties,
            screen);
    };

    auto render_bottom_right = [&, properties](tui::IRect rect) {
        if (doc.page_index + 3 >= doc.page_count) {
            return;
        }

        auto _ = ctx.push("bottom_right_page");
        float this_ratio = float(rect.height) / rect_.height;

        renderer_->render(
            ctx,
            {
                .document_id = doc.document_id,
                .page_index = doc.page_index + 3,
            },
            {
                .rect = rect,
                .subarea =
                    {
                        .x = 0.f,
                        .y = 0.f,
                        .width = 1.f,
                        .height = this_ratio,
                    },
                .placement =
                    {
                        .hor = tui::Alignment::Min,
                        .vert = tui::Alignment::Min,
                    },
            },
            properties,
            screen);
    };

    return tui::vbox({
               tui::hbox({
                   tui::withSize(render_top_left) | tui::flex |
                       tui::size(tui::HEIGHT, tui::EQUAL, 100 - doc.scroll_offset),
                   tui::withSize(render_top_right) | tui::flex |
                       tui::size(tui::HEIGHT, tui::EQUAL, 100 - doc.scroll_offset),
               }) | tui::flex,
               tui::hbox({
                   tui::withSize(render_bottom_left) | tui::flex |
                       tui::size(tui::HEIGHT, tui::EQUAL, doc.scroll_offset),
                   tui::withSize(render_bottom_right) | tui::flex |
                       tui::size(tui::HEIGHT, tui::EQUAL, doc.scroll_offset),
               }) | tui::flex,
           }) |
           tui::reflect(rect_);
}

}  // namespace pdft::app
