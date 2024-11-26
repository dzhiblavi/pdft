#include "pdft/state/app/document_state.h"
#include "pdft/core/log/log.h"

#include <magic_enum.hpp>

namespace pdft::state::app {

void DocumentState::scrollNext(int32_t rows) {
    switch (view_mode_) {
        using enum ViewMode;

        case OnePage:
            nextPage(rows > 0 ? 1 : -1);
            break;

        case TwoPages:
            nextPage(rows > 0 ? 2 : -2);
            break;

        case OnePageContinuous:
        case TwoPagesContinuous:
            scroll_offset_ += rows;
            break;
    }
}

void DocumentState::nextPage(int32_t delta) {
    if (delta >= 0) {
        if (page_index_ + delta < page_count_) {
            page_index_ += delta;
        } else {
            page_index_ = 0;
        }
    } else {
        delta = -delta;

        if (page_index_ >= static_cast<uint32_t>(delta)) {
            page_index_ -= delta;
        } else {
            page_index_ = page_count_ - 1;
        }
    }
}

void DocumentState::setPageIndex(uint32_t index) {
    scroll_offset_ = 0;
    page_index_ = std::min(page_count_ - 1, index);
}

void DocumentState::nextLayout(int32_t delta) {
    auto current_layout_index = static_cast<int>(view_mode_);
    auto new_layout_index = (current_layout_index + delta) % magic_enum::enum_count<ViewMode>();

    scroll_offset_ = 0;
    view_mode_ = static_cast<ViewMode>(new_layout_index);
}

DocumentId DocumentState::getDocumentId() const {
    return document_id_;
}

void DocumentState::setDocument(DocumentId id, uint32_t page_count) {
    document_id_ = id;
    page_count_ = page_count;

    if (page_index_ >= page_count) {
        scroll_offset_ = 0;
        page_index_ = page_count - 1;
    }
}

uint32_t DocumentState::getPageIndex() const {
    return page_index_;
}

uint32_t DocumentState::getPageCount() const {
    return page_count_;
}

void DocumentState::applyScrollOffset(uint32_t height, int32_t pages_per_height) {
    log::debug(
        "applyScrollOffset before: scroll_offset={}, page_index={}", scroll_offset_, page_index_);

    if (scroll_offset_ < 0) {
        int count = 1 + -scroll_offset_ / int(height);
        scroll_offset_ += count * height;
        nextPage(-pages_per_height * count);
    }

    if (scroll_offset_ >= static_cast<int32_t>(height)) {
        int count = scroll_offset_ / int(height);
        scroll_offset_ -= count * height;
        nextPage(pages_per_height * count);
    }

    log::debug(
        "applyScrollOffset after: scroll_offset={}, page_index={}", scroll_offset_, page_index_);
}

tui::Element DocumentState::render(const image::ImageProperties& properties, tui::Context& ctx) {
    auto _ = ctx.push("render");

    switch (view_mode_) {
        using enum ViewMode;

        case OnePage:
            return renderOnePage(properties, ctx);

        case TwoPages:
            return renderTwoPages(properties, ctx);

        case OnePageContinuous:
            return renderOnePageContinuous(properties, ctx);

        case TwoPagesContinuous:
            return renderTwoPagesContinuous(properties, ctx);
    }
}

tui::Element DocumentState::renderOnePage(
    const image::ImageProperties& properties, tui::Context& ctx) {
    auto render = [&](tui::IRect rect) {
        auto _ = ctx.push("only_page");

        renderer_->render(
            rect,
            {
                .hor = tui::Alignment::Center,
                .vert = tui::Alignment::Center,
            },
            properties,
            {
                .document_id = document_id_,
                .page_index = page_index_,
            },
            ctx);
    };

    return tui::withSize(render);
}

tui::Element DocumentState::renderTwoPages(
    const image::ImageProperties& properties, tui::Context& ctx) {
    auto render_left = [&](tui::IRect rect) {
        auto _ = ctx.push("left_page");

        renderer_->render(
            rect,
            {
                .hor = tui::Alignment::Max,
                .vert = tui::Alignment::Center,
            },
            properties,
            {
                .document_id = document_id_,
                .page_index = page_index_,
            },
            ctx);
    };

    auto render_right = [&](tui::IRect rect) {
        if (page_index_ + 1 >= page_count_) {
            return;
        }

        auto _ = ctx.push("right_page");

        renderer_->render(
            rect,
            {
                .hor = tui::Alignment::Min,
                .vert = tui::Alignment::Center,
            },
            properties,
            {
                .document_id = document_id_,
                .page_index = page_index_ + 1,
            },
            ctx);
    };

    return tui::hbox({
        tui::withSize(render_left) | tui::flex,
        tui::withSize(render_right) | tui::flex,
    });
}

tui::Element DocumentState::renderOnePageContinuous(
    const image::ImageProperties& properties, tui::Context& ctx) {
    applyScrollOffset(100, 1);

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
            properties,
            {
                .document_id = document_id_,
                .page_index = page_index_,
            },
            ctx);
    };

    auto render_bottom = [&](tui::IRect rect) {
        if (page_index_ + 1 >= page_count_) {
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
            properties,
            {
                .document_id = document_id_,
                .page_index = page_index_ + 1,
            },
            ctx);
    };

    return tui::vbox({
               tui::withSize(render_top) | tui::flex |
                   tui::size(tui::HEIGHT, tui::EQUAL, 100 - scroll_offset_),
               tui::withSize(render_bottom) | tui::flex |
                   tui::size(tui::HEIGHT, tui::EQUAL, scroll_offset_),
           }) |
           tui::reflect(rect_);
}

tui::Element DocumentState::renderTwoPagesContinuous(
    const image::ImageProperties& properties, tui::Context& ctx) {
    applyScrollOffset(100, 2);

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
            properties,
            {
                .document_id = document_id_,
                .page_index = page_index_,
            },
            ctx);
    };

    auto render_top_right = [&](tui::IRect rect) {
        if (page_index_ + 1 >= page_count_) {
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
            properties,
            {
                .document_id = document_id_,
                .page_index = page_index_ + 1,
            },
            ctx);
    };

    auto render_bottom_left = [&](tui::IRect rect) {
        if (page_index_ + 2 >= page_count_) {
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
            properties,
            {
                .document_id = document_id_,
                .page_index = page_index_ + 2,
            },
            ctx);
    };

    auto render_bottom_right = [&](tui::IRect rect) {
        if (page_index_ + 3 >= page_count_) {
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
            properties,
            {
                .document_id = document_id_,
                .page_index = page_index_ + 3,
            },
            ctx);
    };

    return tui::vbox({
               tui::hbox({
                   tui::withSize(render_top_left) | tui::flex |
                       tui::size(tui::HEIGHT, tui::EQUAL, 100 - scroll_offset_),
                   tui::withSize(render_top_right) | tui::flex |
                       tui::size(tui::HEIGHT, tui::EQUAL, 100 - scroll_offset_),
               }) | tui::flex,
               tui::hbox({
                   tui::withSize(render_bottom_left) | tui::flex |
                       tui::size(tui::HEIGHT, tui::EQUAL, scroll_offset_),
                   tui::withSize(render_bottom_right) | tui::flex |
                       tui::size(tui::HEIGHT, tui::EQUAL, scroll_offset_),
               }) | tui::flex,
           }) |
           tui::reflect(rect_);
}

}  // namespace pdft::state::app
