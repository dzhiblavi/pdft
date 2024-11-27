#include "pdft/view/app/document_view.h"

namespace pdft::view::app {

tui::Element DocumentView::render(
    const state::DocumentState& state, const image::ImageProperties& image, tui::Context& ctx) {
    auto _ = ctx.push("render");

    switch (state.view_mode) {
        using enum state::ViewMode;

        case OnePage:
            return renderOnePage(state, image, ctx);

        case TwoPages:
            return renderTwoPages(state, image, ctx);

        case OnePageContinuous:
            return renderOnePageContinuous(state, image, ctx);

        case TwoPagesContinuous:
            return renderTwoPagesContinuous(state, image, ctx);
    }
}

tui::Element DocumentView::renderOnePage(
    const state::DocumentState& state, const image::ImageProperties& image, tui::Context& ctx) {
    auto render = [&](tui::IRect rect) {
        auto _ = ctx.push("only_page");

        renderer_->render(
            rect,
            {
                .hor = tui::Alignment::Center,
                .vert = tui::Alignment::Center,
            },
            image,
            {
                .document_id = state.document_id,
                .page_index = state.page_index,
            },
            ctx);
    };

    return tui::withSize(render);
}

tui::Element DocumentView::renderTwoPages(
    const state::DocumentState& state, const image::ImageProperties& image, tui::Context& ctx) {
    auto render_left = [&](tui::IRect rect) {
        auto _ = ctx.push("left_page");

        renderer_->render(
            rect,
            {
                .hor = tui::Alignment::Max,
                .vert = tui::Alignment::Center,
            },
            image,
            {
                .document_id = state.document_id,
                .page_index = state.page_index,
            },
            ctx);
    };

    auto render_right = [&](tui::IRect rect) {
        if (state.page_index + 1 >= state.page_count) {
            return;
        }

        auto _ = ctx.push("right_page");

        renderer_->render(
            rect,
            {
                .hor = tui::Alignment::Min,
                .vert = tui::Alignment::Center,
            },
            image,
            {
                .document_id = state.document_id,
                .page_index = state.page_index + 1,
            },
            ctx);
    };

    return tui::hbox({
        tui::withSize(render_left) | tui::flex,
        tui::withSize(render_right) | tui::flex,
    });
}

tui::Element DocumentView::renderOnePageContinuous(
    const state::DocumentState& state, const image::ImageProperties& image, tui::Context& ctx) {
    auto render_top = [&](tui::IRect rect) {
        auto _ = ctx.push("top_page");
        float this_ratio = float(rect.height) / rect_.height;

        renderer_->render(
            rect,
            {
                .x = 0.f,
                .y = 1.f - this_ratio,
                .width = 1.f,
                .height = this_ratio,
            },
            {
                .hor = tui::Alignment::Center,
                .vert = tui::Alignment::Max,
            },
            image,
            {
                .document_id = state.document_id,
                .page_index = state.page_index,
            },
            ctx);
    };

    auto render_bottom = [&](tui::IRect rect) {
        if (state.page_index + 1 >= state.page_count) {
            return;
        }
        auto _ = ctx.push("bottom_page");
        float this_ratio = float(rect.height) / rect_.height;

        renderer_->render(
            rect,
            {
                .x = 0.f,
                .y = 0.f,
                .width = 1.f,
                .height = this_ratio,
            },
            {
                .hor = tui::Alignment::Center,
                .vert = tui::Alignment::Min,
            },
            image,
            {
                .document_id = state.document_id,
                .page_index = state.page_index + 1,
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
    const state::DocumentState& state, const image::ImageProperties& image, tui::Context& ctx) {
    auto render_top_left = [&](tui::IRect rect) {
        auto _ = ctx.push("top_left_page");
        float this_ratio = float(rect.height) / rect_.height;

        renderer_->render(
            rect,
            {
                .x = 0.f,
                .y = 1.f - this_ratio,
                .width = 1.f,
                .height = this_ratio,
            },
            {
                .hor = tui::Alignment::Max,
                .vert = tui::Alignment::Max,
            },
            image,
            {
                .document_id = state.document_id,
                .page_index = state.page_index,
            },
            ctx);
    };

    auto render_top_right = [&](tui::IRect rect) {
        if (state.page_index + 1 >= state.page_count) {
            return;
        }

        auto _ = ctx.push("top_right_page");
        float this_ratio = float(rect.height) / rect_.height;

        renderer_->render(
            rect,
            {
                .x = 0.f,
                .y = 1.f - this_ratio,
                .width = 1.f,
                .height = this_ratio,
            },
            {
                .hor = tui::Alignment::Min,
                .vert = tui::Alignment::Max,
            },
            image,
            {
                .document_id = state.document_id,
                .page_index = state.page_index + 1,
            },
            ctx);
    };

    auto render_bottom_left = [&](tui::IRect rect) {
        if (state.page_index + 2 >= state.page_count) {
            return;
        }

        auto _ = ctx.push("bottom_left_page");
        float this_ratio = float(rect.height) / rect_.height;

        renderer_->render(
            rect,
            {
                .x = 0.f,
                .y = 0.f,
                .width = 1.f,
                .height = this_ratio,
            },
            {
                .hor = tui::Alignment::Max,
                .vert = tui::Alignment::Min,
            },
            image,
            {
                .document_id = state.document_id,
                .page_index = state.page_index + 2,
            },
            ctx);
    };

    auto render_bottom_right = [&](tui::IRect rect) {
        if (state.page_index + 3 >= state.page_count) {
            return;
        }

        auto _ = ctx.push("bottom_right_page");
        float this_ratio = float(rect.height) / rect_.height;

        renderer_->render(
            rect,
            {
                .x = 0.f,
                .y = 0.f,
                .width = 1.f,
                .height = this_ratio,
            },
            {
                .hor = tui::Alignment::Min,
                .vert = tui::Alignment::Min,
            },
            image,
            {
                .document_id = state.document_id,
                .page_index = state.page_index + 3,
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
