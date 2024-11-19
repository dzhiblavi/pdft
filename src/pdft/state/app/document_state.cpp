#include "pdft/state/app/document_state.h"
#include "pdft/core/log/log.h"
#include "pdft/view/layout.h"

#include <magic_enum.hpp>

namespace pdft::state::app {

void DocumentState::scrollNext(int32_t delta) {
    switch (view_mode_) {
        using enum ViewMode;

        case OnePage:
            nextPage(+1);
            break;

        case TwoPages:
            nextPage(+2);
            break;

        case OnePageContinuous:
        case TwoPagesContinuous:
            scroll_offset_ += 10 * delta;
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

        if (page_index_ >= delta) {
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
    constexpr float MinZoom = 0.1f;
    constexpr float MaxZoom = 2.0f;
    zoom_ = std::fmin(MaxZoom, std::fmax(MinZoom, coeff));
}

DocumentId DocumentState::getDocumentId() const {
    return document_id_;
}

void DocumentState::setDocument(DocumentId id, uint32_t page_count) {
    if (page_index_ >= page_count) {
        scroll_offset_ = 0;
        page_index_ = page_count - 1;
    }
}

void DocumentState::applyScrollOffset(uint32_t height, int32_t pages_per_height) {
    core::log::global()->info(
        "applyScrollOffset before: scroll_offset={}, page_index={}", scroll_offset_, page_index_);

    if (scroll_offset_ < 0) {
        int count = 1 + -scroll_offset_ / int(height);
        scroll_offset_ += count * height;
        nextPage(-pages_per_height * count);
    }

    if (scroll_offset_ >= height) {
        int count = scroll_offset_ / int(height);
        scroll_offset_ -= count * height;
        nextPage(pages_per_height * count);
    }

    core::log::global()->info(
        "applyScrollOffset after: scroll_offset={}, page_index={}", scroll_offset_, page_index_);
}

void DocumentState::getLayout(view::IRect rect, std::vector<view::Placement>& layout) {
    switch (view_mode_) {
        using enum ViewMode;

        case OnePage:
            return getLayoutOnePage(rect, layout);

        case TwoPages:
            return getLayoutTwoPages(rect, layout);

        case OnePageContinuous:
            return getLayoutOnePageContinuous(rect, layout);

        case TwoPagesContinuous:
            return getLayoutTwoPagesContinuous(rect, layout);
    }
}

void DocumentState::getLayoutOnePage(view::IRect rect, std::vector<view::Placement>& layout) {
    layout.push_back({
        .position =
            {
                .rect = rect,
                .hor = view::AxisFit::Center,
                .vert = view::AxisFit::Center,
            },
        .document_id = document_id_,
        .page_index = page_index_,
    });
}

void DocumentState::getLayoutTwoPages(view::IRect rect, std::vector<view::Placement>& layout) {
    auto rects =
        view::horizontal(
            view::constraints::Fraction{0.15f / zoom_},
            view::constraints::Fill{1},
            view::constraints::Length{1},
            view::constraints::Fill{1},
            view::constraints::Fraction{0.15f / zoom_})
            .split(rect);

    layout.push_back({
        .position =
            {
                .rect = rects[1],
                .hor = view::AxisFit::Max,
                .vert = view::AxisFit::Center,
            },
        .document_id = document_id_,
        .page_index = page_index_,
    });

    if (page_index_ + 1 >= page_count_) {
        return;
    }

    layout.push_back({
        .position =
            {
                .rect = rects[3],
                .hor = view::AxisFit::Min,
                .vert = view::AxisFit::Center,
            },
        .document_id = document_id_,
        .page_index = page_index_ + 1,
    });
}

void DocumentState::getLayoutOnePageContinuous(
    view::IRect rect, std::vector<view::Placement>& layout) {
    applyScrollOffset(rect.height, 1);

    rect = view::horizontal(
               view::constraints::Fraction{0.10f / zoom_},
               view::constraints::Fill{1},
               view::constraints::Fraction{0.10f / zoom_})
               .split(rect)[1];

    auto top_height = rect.height - scroll_offset_;
    auto top = offsetRect(rect, 0, -scroll_offset_);
    auto bottom = offsetRect(rect, 0, +top_height);

    layout.push_back({
        .position =
            {
                .rect = top,
                .hor = view::AxisFit::Center,
                .vert = view::AxisFit::Max,
            },
        .document_id = document_id_,
        .page_index = page_index_,
    });

    if (page_index_ + 1 >= page_count_) {
        return;
    }

    layout.push_back({
        .position =
            {
                .rect = bottom,
                .hor = view::AxisFit::Center,
                .vert = view::AxisFit::Min,
            },
        .document_id = document_id_,
        .page_index = page_index_ + 1,
    });
}

void DocumentState::getLayoutTwoPagesContinuous(
    view::IRect rect, std::vector<view::Placement>& layout) {
    applyScrollOffset(rect.height, 2);

    auto rects =
        view::horizontal(
            view::constraints::Fraction{0.05f / zoom_},
            view::constraints::Fill{1},  // rects[1]
            view::constraints::Fill{1},  // rects[2]
            view::constraints::Fraction{0.05f / zoom_})
            .split(rect);

    uint32_t top_height = rect.height - scroll_offset_;
    auto top_left = offsetRect(rects[1], 0, -scroll_offset_);
    auto top_right = offsetRect(rects[2], 0, -scroll_offset_);
    auto bottom_left = offsetRect(rects[1], 0, +top_height);
    auto bottom_right = offsetRect(rects[2], 0, +top_height);

    layout.push_back({
        .position =
            {
                .rect = top_left,
                .hor = view::AxisFit::Max,
                .vert = view::AxisFit::Max,
            },
        .document_id = document_id_,
        .page_index = page_index_,
    });

    if (page_index_ + 1 >= page_count_) {
        return;
    }

    layout.push_back({
        .position =
            {
                .rect = top_right,
                .hor = view::AxisFit::Min,
                .vert = view::AxisFit::Max,
            },
        .document_id = document_id_,
        .page_index = page_index_ + 1,
    });

    if (page_index_ + 2 >= page_count_) {
        return;
    }

    layout.push_back({
        .position =
            {
                .rect = bottom_left,
                .hor = view::AxisFit::Max,
                .vert = view::AxisFit::Min,
            },
        .document_id = document_id_,
        .page_index = page_index_ + 2,
    });

    if (page_index_ + 3 >= page_count_) {
        return;
    }

    layout.push_back({
        .position =
            {
                .rect = bottom_right,
                .hor = view::AxisFit::Min,
                .vert = view::AxisFit::Min,
            },
        .document_id = document_id_,
        .page_index = page_index_ + 3,
    });
}

}  // namespace pdft::state::app
