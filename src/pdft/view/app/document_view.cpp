#include "pdft/view/app/document_view.h"

namespace pdft::view::app {

tui::Element DocumentView::render(
    const state::DocumentState& state, float scaling_coeff, bool invert_colors, tui::Context& ctx) {
    auto _ = ctx.push("render");

    switch (state.view_mode) {
        using enum state::ViewMode;

        case OnePage:
            return renderOnePage(state, scaling_coeff, invert_colors, ctx);

        case TwoPages:
            return renderTwoPages(state, scaling_coeff, invert_colors, ctx);

        case OnePageContinuous:
            return renderOnePageContinuous(state, scaling_coeff, invert_colors, ctx);

        case TwoPagesContinuous:
            return renderTwoPagesContinuous(state, scaling_coeff, invert_colors, ctx);
    }
}

tui::Element DocumentView::renderOnePage(
    const state::DocumentState& state, float scaling_coeff, bool invert_colors, tui::Context& ctx) {
    auto render = [&, scaling_coeff, invert_colors](tui::IRect rect) {
        auto _ = ctx.push("only_page");

        renderer_->render(
            {
                .document_id = state.document_id,
                .page_index = state.page_index,
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
            ctx);
    };

    return tui::withSize(render);
}

tui::Element DocumentView::renderTwoPages(
    const state::DocumentState& state, float scaling_coeff, bool invert_colors, tui::Context& ctx) {
    auto render_left = [&, scaling_coeff, invert_colors](tui::IRect rect) {
        auto _ = ctx.push("left_page");

        renderer_->render(
            {
                .document_id = state.document_id,
                .page_index = state.page_index,
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
            ctx);
    };

    auto render_right = [&, scaling_coeff, invert_colors](tui::IRect rect) {
        if (state.page_index + 1 >= state.page_count) {
            return;
        }

        auto _ = ctx.push("right_page");

        renderer_->render(
            {
                .document_id = state.document_id,
                .page_index = state.page_index + 1,
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
            ctx);
    };

    return tui::hbox({
        tui::withSize(render_left) | tui::flex,
        tui::withSize(render_right) | tui::flex,
    });
}

tui::Element DocumentView::renderOnePageContinuous(
    const state::DocumentState& state, float scaling_coeff, bool invert_colors, tui::Context& ctx) {
    auto render_top = [&, scaling_coeff, invert_colors](tui::IRect rect) {
        auto _ = ctx.push("top_page");
        float this_ratio = float(rect.height) / rect_.height;

        renderer_->render(
            {
                .document_id = state.document_id,
                .page_index = state.page_index,
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
            ctx);
    };

    auto render_bottom = [&, scaling_coeff, invert_colors](tui::IRect rect) {
        if (state.page_index + 1 >= state.page_count) {
            return;
        }
        auto _ = ctx.push("bottom_page");
        float this_ratio = float(rect.height) / rect_.height;

        renderer_->render(
            {
                .document_id = state.document_id,
                .page_index = state.page_index + 1,
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
            ctx);
    };

    return tui::vbox({
               tui::withSize(render_top) | tui::flex |
                   tui::size(tui::HEIGHT, tui::EQUAL, 100 - state.scroll_offset),
               tui::withSize(render_bottom) | tui::flex |
                   tui::size(tui::HEIGHT, tui::EQUAL, state.scroll_offset),
           }) |
           tui::reflect(rect_);
}

tui::Element DocumentView::renderTwoPagesContinuous(
    const state::DocumentState& state, float scaling_coeff, bool invert_colors, tui::Context& ctx) {
    auto render_top_left = [&, scaling_coeff, invert_colors](tui::IRect rect) {
        auto _ = ctx.push("top_left_page");
        float this_ratio = float(rect.height) / rect_.height;

        renderer_->render(
            {
                .document_id = state.document_id,
                .page_index = state.page_index,
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
            ctx);
    };

    auto render_top_right = [&, scaling_coeff, invert_colors](tui::IRect rect) {
        if (state.page_index + 1 >= state.page_count) {
            return;
        }

        auto _ = ctx.push("top_right_page");
        float this_ratio = float(rect.height) / rect_.height;

        renderer_->render(
            {
                .document_id = state.document_id,
                .page_index = state.page_index + 1,
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
            ctx);
    };

    auto render_bottom_left = [&, scaling_coeff, invert_colors](tui::IRect rect) {
        if (state.page_index + 2 >= state.page_count) {
            return;
        }

        auto _ = ctx.push("bottom_left_page");
        float this_ratio = float(rect.height) / rect_.height;

        renderer_->render(
            {
                .document_id = state.document_id,
                .page_index = state.page_index + 2,
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
            ctx);
    };

    auto render_bottom_right = [&, scaling_coeff, invert_colors](tui::IRect rect) {
        if (state.page_index + 3 >= state.page_count) {
            return;
        }

        auto _ = ctx.push("bottom_right_page");
        float this_ratio = float(rect.height) / rect_.height;

        renderer_->render(
            {
                .document_id = state.document_id,
                .page_index = state.page_index + 3,
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
            ctx);
    };

    return tui::vbox({
               tui::hbox({
                   tui::withSize(render_top_left) | tui::flex |
                       tui::size(tui::HEIGHT, tui::EQUAL, 100 - state.scroll_offset),
                   tui::withSize(render_top_right) | tui::flex |
                       tui::size(tui::HEIGHT, tui::EQUAL, 100 - state.scroll_offset),
               }) | tui::flex,
               tui::hbox({
                   tui::withSize(render_bottom_left) | tui::flex |
                       tui::size(tui::HEIGHT, tui::EQUAL, state.scroll_offset),
                   tui::withSize(render_bottom_right) | tui::flex |
                       tui::size(tui::HEIGHT, tui::EQUAL, state.scroll_offset),
               }) | tui::flex,
           }) |
           tui::reflect(rect_);
}

}  // namespace pdft::view::app
