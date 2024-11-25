#include "pdft/state/app/document_state.h"
#include "pdft/core/log/log.h"
#include "pdft/tui/layout/layout.h"

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

void DocumentState::updateZoom(float coeff) {
    constexpr float MinZoom = 0.001f;
    constexpr float MaxZoom = 200.0f;
    zoom_ = std::fmin(MaxZoom, std::fmax(MinZoom, zoom_ * coeff));
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

asio::awaitable<void> DocumentState::render(
    tui::layout::IRect rect, const image::ImageProperties& properties, tui::ContextRef ctx) {
    switch (view_mode_) {
        using enum ViewMode;

        case OnePage:
            co_return co_await renderOnePage(rect, properties, ctx);

        case TwoPages:
            co_return co_await renderTwoPages(rect, properties, ctx);

        case OnePageContinuous:
            co_return co_await renderOnePageContinuous(rect, properties, ctx);

        case TwoPagesContinuous:
            co_return co_await renderTwoPagesContinuous(rect, properties, ctx);
    }
}

asio::awaitable<void> DocumentState::renderOnePage(
    tui::layout::IRect rect, const image::ImageProperties& properties, tui::ContextRef ctx) {
    co_await renderer_->render(
        rect,
        {
            .hor = tui::layout::Alignment::Center,
            .vert = tui::layout::Alignment::Center,
        },
        properties,
        {
            .document_id = document_id_,
            .page_index = page_index_,
        },
        ctx);
}

asio::awaitable<void> DocumentState::renderTwoPages(
    tui::layout::IRect rect, const image::ImageProperties& properties, tui::ContextRef ctx) {
    using namespace tui::layout;
    auto rects =
        horizontal(
            constraints::Fraction{0.15f / zoom_},
            constraints::Fill{1},
            constraints::Length{1},
            constraints::Fill{1},
            constraints::Fraction{0.15f / zoom_})
            .split(rect);

    co_await renderer_->render(
        rects[1],
        {
            .hor = tui::layout::Alignment::Max,
            .vert = tui::layout::Alignment::Center,
        },
        properties,
        {
            .document_id = document_id_,
            .page_index = page_index_,
        },
        ctx);

    if (page_index_ + 1 >= page_count_) {
        co_return;
    }

    co_await renderer_->render(
        rects[3],
        {
            .hor = tui::layout::Alignment::Min,
            .vert = tui::layout::Alignment::Center,
        },
        properties,
        {
            .document_id = document_id_,
            .page_index = page_index_ + 1,
        },
        ctx);
}

asio::awaitable<void> DocumentState::renderOnePageContinuous(
    tui::layout::IRect rect, const image::ImageProperties& properties, tui::ContextRef ctx) {
    using namespace tui::layout;
    applyScrollOffset(rect.height, 1);

    log::debug("zoom is {}, pad fraction is {}", zoom_, std::fmin(0.47f, 0.30f / zoom_));

    rect = horizontal(
               constraints::Fraction{std::fmin(0.47f, 0.30f / zoom_)},
               constraints::Fill{1},
               constraints::Fraction{std::fmin(0.47f, 0.30f / zoom_)})
               .split(rect)[1];

    auto top_height = rect.height - scroll_offset_;

    auto page_rects = vertical(constraints::Length(top_height), constraints::Fill(1)).split(rect);

    co_await renderer_->render(
        page_rects[0],
        {
            .hor = tui::layout::Alignment::Center,
            .vert = tui::layout::Alignment::Max,
            .fit = tui::image::Placement::FitMode::CropAndFill,
        },
        properties,
        {
            .document_id = document_id_,
            .page_index = page_index_,
        },
        ctx);

    if (page_index_ + 1 >= page_count_) {
        co_return;
    }

    co_await renderer_->render(
        page_rects[1],
        {
            .hor = tui::layout::Alignment::Center,
            .vert = tui::layout::Alignment::Min,
            .fit = tui::image::Placement::FitMode::CropAndFill,
        },
        properties,
        {
            .document_id = document_id_,
            .page_index = page_index_ + 1,
        },
        ctx);
}

asio::awaitable<void> DocumentState::renderTwoPagesContinuous(
    tui::layout::IRect rect, const image::ImageProperties& properties, tui::ContextRef ctx) {
    using namespace tui::layout;

    applyScrollOffset(rect.height, 2);

    auto rects =
        horizontal(
            constraints::Fraction{std::fmin(0.47f, 0.1f / zoom_)},
            constraints::Fill{1},  // rects[1]
            constraints::Fill{1},  // rects[2]
            constraints::Fraction{std::fmin(0.47f, 0.10f / zoom_)})
            .split(rect);

    uint32_t top_height = rect.height - scroll_offset_;

    auto left_rects =
        vertical(constraints::Length(top_height), constraints::Fill(1)).split(rects[1]);
    auto right_rects =
        vertical(constraints::Length(top_height), constraints::Fill(1)).split(rects[2]);

    co_await renderer_->render(
        left_rects[0],
        {
            .hor = tui::layout::Alignment::Max,
            .vert = tui::layout::Alignment::Max,
            .fit = tui::image::Placement::FitMode::CropAndFill,
        },
        properties,
        {
            .document_id = document_id_,
            .page_index = page_index_,
        },
        ctx);

    if (page_index_ + 1 >= page_count_) {
        co_return;
    }

    co_await renderer_->render(
        right_rects[0],
        {
            .hor = tui::layout::Alignment::Min,
            .vert = tui::layout::Alignment::Max,
            .fit = tui::image::Placement::FitMode::CropAndFill,
        },
        properties,
        {
            .document_id = document_id_,
            .page_index = page_index_ + 1,
        },
        ctx);

    if (page_index_ + 2 >= page_count_) {
        co_return;
    }

    co_await renderer_->render(
        left_rects[1],
        {
            .hor = tui::layout::Alignment::Max,
            .vert = tui::layout::Alignment::Min,
            .fit = tui::image::Placement::FitMode::CropAndFill,
        },
        properties,
        {
            .document_id = document_id_,
            .page_index = page_index_ + 2,
        },
        ctx);

    if (page_index_ + 3 >= page_count_) {
        co_return;
    }

    co_await renderer_->render(
        right_rects[1],
        {
            .hor = tui::layout::Alignment::Min,
            .vert = tui::layout::Alignment::Min,
            .fit = tui::image::Placement::FitMode::CropAndFill,
        },
        properties,
        {
            .document_id = document_id_,
            .page_index = page_index_ + 3,
        },
        ctx);
}

}  // namespace pdft::state::app
