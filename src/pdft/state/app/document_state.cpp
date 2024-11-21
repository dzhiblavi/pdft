#include "pdft/state/app/document_state.h"
#include "pdft/core/log/log.h"
#include "pdft/view/layout.h"

#include <magic_enum.hpp>

namespace pdft::state::app {

void DocumentState::scrollNext(int32_t delta) {
    switch (view_mode_) {
        using enum ViewMode;

        case OnePage:
            nextPage(delta);
            break;

        case TwoPages:
            nextPage(2 * delta);
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

void DocumentState::render(view::IRect rect, core::io::OutBuffer& buffer) {
    switch (view_mode_) {
        using enum ViewMode;

        case OnePage:
            return renderOnePage(rect, buffer);

        case TwoPages:
            return renderTwoPages(rect, buffer);

        case OnePageContinuous:
            return renderOnePageContinuous(rect, buffer);

        case TwoPagesContinuous:
            return renderTwoPagesContinuous(rect, buffer);
    }
}

void DocumentState::renderOnePage(view::IRect rect, core::io::OutBuffer& buffer) {
    renderer_->render(
        buffer,
        rect,
        {
            .hor = view::AxisFit::Center,
            .vert = view::AxisFit::Center,
        },
        {
            .document_id = document_id_,
            .page_index = page_index_,
        });
}

void DocumentState::renderTwoPages(view::IRect rect, core::io::OutBuffer& buffer) {
    auto rects =
        view::horizontal(
            view::constraints::Fraction{0.15f / zoom_},
            view::constraints::Fill{1},
            view::constraints::Length{1},
            view::constraints::Fill{1},
            view::constraints::Fraction{0.15f / zoom_})
            .split(rect);

    renderer_->render(
        buffer,
        rects[1],
        {
            .hor = view::AxisFit::Max,
            .vert = view::AxisFit::Center,
        },
        {
            .document_id = document_id_,
            .page_index = page_index_,
        });

    if (page_index_ + 1 >= page_count_) {
        return;
    }

    renderer_->render(
        buffer,
        rects[3],
        {
            .hor = view::AxisFit::Min,
            .vert = view::AxisFit::Center,
        },
        {
            .document_id = document_id_,
            .page_index = page_index_ + 1,
        });
}

void DocumentState::renderOnePageContinuous(view::IRect rect, core::io::OutBuffer& buffer) {
    applyScrollOffset(rect.height, 1);

    core::log::global()->info(
        "zoom is {}, pad fraction is {}", zoom_, std::fmin(0.47f, 0.30f / zoom_));

    rect = view::horizontal(
               view::constraints::Fraction{std::fmin(0.47f, 0.30f / zoom_)},
               view::constraints::Fill{1},
               view::constraints::Fraction{std::fmin(0.47f, 0.30f / zoom_)})
               .split(rect)[1];

    auto top_height = rect.height - scroll_offset_;

    auto page_rects =
        view::vertical(view::constraints::Length(top_height), view::constraints::Fill(1))
            .split(rect);

    renderer_->render(
        buffer,
        page_rects[0],
        {
            .hor = view::AxisFit::Center,
            .vert = view::AxisFit::Max,
            .fit = view::PagePlacement::FitMode::CropAndFill,
        },
        {
            .document_id = document_id_,
            .page_index = page_index_,
        });

    if (page_index_ + 1 >= page_count_) {
        return;
    }

    renderer_->render(
        buffer,
        page_rects[1],
        {
            .hor = view::AxisFit::Center,
            .vert = view::AxisFit::Min,
            .fit = view::PagePlacement::FitMode::CropAndFill,
        },
        {
            .document_id = document_id_,
            .page_index = page_index_ + 1,
        });
}

void DocumentState::renderTwoPagesContinuous(view::IRect rect, core::io::OutBuffer& buffer) {
    applyScrollOffset(rect.height, 2);

    auto rects =
        view::horizontal(
            view::constraints::Fraction{std::fmin(0.47f, 0.1f / zoom_)},
            view::constraints::Fill{1},  // rects[1]
            view::constraints::Fill{1},  // rects[2]
            view::constraints::Fraction{std::fmin(0.47f, 0.10f / zoom_)})
            .split(rect);

    uint32_t top_height = rect.height - scroll_offset_;

    auto left_rects =
        view::vertical(view::constraints::Length(top_height), view::constraints::Fill(1))
            .split(rects[1]);
    auto right_rects =
        view::vertical(view::constraints::Length(top_height), view::constraints::Fill(1))
            .split(rects[2]);

    renderer_->render(
        buffer,
        left_rects[0],
        {
            .hor = view::AxisFit::Max,
            .vert = view::AxisFit::Max,
            .fit = view::PagePlacement::FitMode::CropAndFill,
        },
        {
            .document_id = document_id_,
            .page_index = page_index_,
        });

    if (page_index_ + 1 >= page_count_) {
        return;
    }

    renderer_->render(
        buffer,
        right_rects[0],
        {
            .hor = view::AxisFit::Min,
            .vert = view::AxisFit::Max,
            .fit = view::PagePlacement::FitMode::CropAndFill,
        },
        {
            .document_id = document_id_,
            .page_index = page_index_ + 1,
        });

    if (page_index_ + 2 >= page_count_) {
        return;
    }

    renderer_->render(
        buffer,
        left_rects[1],
        {
            .hor = view::AxisFit::Max,
            .vert = view::AxisFit::Min,
            .fit = view::PagePlacement::FitMode::CropAndFill,
        },
        {
            .document_id = document_id_,
            .page_index = page_index_ + 2,
        });

    if (page_index_ + 3 >= page_count_) {
        return;
    }

    renderer_->render(
        buffer,
        right_rects[1],
        {
            .hor = view::AxisFit::Min,
            .vert = view::AxisFit::Min,
            .fit = view::PagePlacement::FitMode::CropAndFill,
        },
        {
            .document_id = document_id_,
            .page_index = page_index_ + 3,
        });
}

}  // namespace pdft::state::app
