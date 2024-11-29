#include "pdft/app/view/impl/document_view.h"

namespace pdft::app::view::impl {

tui::Element DocumentView::render(
    const state::DocumentState& doc,
    float scaling_coeff,
    bool invert_colors,
    tui::display::State& state,
    tui::Context& ctx) {
    auto _ = ctx.push("render");

    switch (doc.view_mode) {
        using enum state::ViewMode;

        case OnePage:
            return renderOnePage(doc, scaling_coeff, invert_colors, state, ctx);

        case TwoPages:
            return renderTwoPages(doc, scaling_coeff, invert_colors, state, ctx);

        case OnePageContinuous:
            return renderOnePageContinuous(doc, scaling_coeff, invert_colors, state, ctx);

        case TwoPagesContinuous:
            return renderTwoPagesContinuous(doc, scaling_coeff, invert_colors, state, ctx);
    }
}

tui::Element DocumentView::renderOnePage(
    const state::DocumentState& doc,
    float scaling_coeff,
    bool invert_colors,
    tui::display::State& state,
    tui::Context& ctx) {
    auto render = [&, scaling_coeff, invert_colors](tui::IRect rect) {
        auto _ = ctx.push("only_page");

        renderer_->render(
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
                .scaling_coeff = scaling_coeff,
                .invert_colors = invert_colors,
            },
            state,
            ctx);
    };

    return tui::withSize(render);
}

tui::Element DocumentView::renderTwoPages(
    const state::DocumentState& doc,
    float scaling_coeff,
    bool invert_colors,
    tui::display::State& state,
    tui::Context& ctx) {
    auto render_left = [&, scaling_coeff, invert_colors](tui::IRect rect) {
        auto _ = ctx.push("left_page");

        renderer_->render(
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
                .scaling_coeff = scaling_coeff,
                .invert_colors = invert_colors,
            },
            state,
            ctx);
    };

    auto render_right = [&, scaling_coeff, invert_colors](tui::IRect rect) {
        if (doc.page_index + 1 >= doc.page_count) {
            return;
        }

        auto _ = ctx.push("right_page");

        renderer_->render(
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
                .scaling_coeff = scaling_coeff,
                .invert_colors = invert_colors,
            },
            state,
            ctx);
    };

    return tui::hbox({
        tui::withSize(render_left) | tui::flex,
        tui::withSize(render_right) | tui::flex,
    });
}

tui::Element DocumentView::renderOnePageContinuous(
    const state::DocumentState& doc,
    float scaling_coeff,
    bool invert_colors,
    tui::display::State& state,
    tui::Context& ctx) {
    auto render_top = [&, scaling_coeff, invert_colors](tui::IRect rect) {
        auto _ = ctx.push("top_page");
        float this_ratio = float(rect.height) / rect_.height;

        renderer_->render(
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
                .scaling_coeff = scaling_coeff,
                .invert_colors = invert_colors,
            },
            state,
            ctx);
    };

    auto render_bottom = [&, scaling_coeff, invert_colors](tui::IRect rect) {
        if (doc.page_index + 1 >= doc.page_count) {
            return;
        }
        auto _ = ctx.push("bottom_page");
        float this_ratio = float(rect.height) / rect_.height;

        renderer_->render(
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
                .scaling_coeff = scaling_coeff,
                .invert_colors = invert_colors,
            },
            state,
            ctx);
    };

    return tui::vbox({
               tui::withSize(render_top) | tui::flex |
                   tui::size(tui::HEIGHT, tui::EQUAL, 100 - doc.scroll_offset),
               tui::withSize(render_bottom) | tui::flex |
                   tui::size(tui::HEIGHT, tui::EQUAL, doc.scroll_offset),
           }) |
           tui::reflect(rect_);
}

tui::Element DocumentView::renderTwoPagesContinuous(
    const state::DocumentState& doc,
    float scaling_coeff,
    bool invert_colors,
    tui::display::State& state,
    tui::Context& ctx) {
    auto render_top_left = [&, scaling_coeff, invert_colors](tui::IRect rect) {
        auto _ = ctx.push("top_left_page");
        float this_ratio = float(rect.height) / rect_.height;

        renderer_->render(
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
                .scaling_coeff = scaling_coeff,
                .invert_colors = invert_colors,
            },
            state,
            ctx);
    };

    auto render_top_right = [&, scaling_coeff, invert_colors](tui::IRect rect) {
        if (doc.page_index + 1 >= doc.page_count) {
            return;
        }

        auto _ = ctx.push("top_right_page");
        float this_ratio = float(rect.height) / rect_.height;

        renderer_->render(
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
                .scaling_coeff = scaling_coeff,
                .invert_colors = invert_colors,
            },
            state,
            ctx);
    };

    auto render_bottom_left = [&, scaling_coeff, invert_colors](tui::IRect rect) {
        if (doc.page_index + 2 >= doc.page_count) {
            return;
        }

        auto _ = ctx.push("bottom_left_page");
        float this_ratio = float(rect.height) / rect_.height;

        renderer_->render(
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
                .scaling_coeff = scaling_coeff,
                .invert_colors = invert_colors,
            },
            state,
            ctx);
    };

    auto render_bottom_right = [&, scaling_coeff, invert_colors](tui::IRect rect) {
        if (doc.page_index + 3 >= doc.page_count) {
            return;
        }

        auto _ = ctx.push("bottom_right_page");
        float this_ratio = float(rect.height) / rect_.height;

        renderer_->render(
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
                .scaling_coeff = scaling_coeff,
                .invert_colors = invert_colors,
            },
            state,
            ctx);
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

}  // namespace pdft::app::view::impl
