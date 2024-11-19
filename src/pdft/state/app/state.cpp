#include "pdft/state/app/state.h"
#include "pdft/core/assert.h"
#include "pdft/core/log/log.h"
#include "pdft/utils/visit.h"
#include "pdft/view/layout.h"

#include <algorithm>

namespace pdft::state::app {

void State::getLayout(view::IRect rect, std::vector<view::Placement>& layout) {
    layout.clear();

    if (auto* state = getState()) {
        utils::visit(state->page_state, [&](auto& page_state) {
            getLayout(page_state, *state, rect, layout);
        });
    }
}

void State::getLayout(
    OnePage, ViewState& state, view::IRect rect, std::vector<view::Placement>& layout) {
    auto doc_id = *getDocumentId();
    auto page_count = doc_storage_->getDocument(doc_id).pageCount();

    layout.push_back({
        .position =
            {
                .rect = rect,
                .hor = view::AxisFit::Center,
                .vert = view::AxisFit::Center,
            },
        .document_id = doc_id,
        .page_index = state.page_index,
    });
}

void State::getLayout(
    TwoPages, ViewState& state, view::IRect rect, std::vector<view::Placement>& layout) {
    auto doc_id = *getDocumentId();
    auto page_count = doc_storage_->getDocument(doc_id).pageCount();

    auto rects =
        view::horizontal(
            view::constraints::Fraction{0.15f / state.zoom},
            view::constraints::Fill{1},
            view::constraints::Length{1},
            view::constraints::Fill{1},
            view::constraints::Fraction{0.15f / state.zoom})
            .split(rect);

    layout.push_back({
        .position =
            {
                .rect = rects[1],
                .hor = view::AxisFit::Max,
                .vert = view::AxisFit::Center,
            },
        .document_id = doc_id,
        .page_index = state.page_index,
    });

    if (state.page_index + 1 >= page_count) {
        return;
    }

    layout.push_back({
        .position =
            {
                .rect = rects[3],
                .hor = view::AxisFit::Min,
                .vert = view::AxisFit::Center,
            },
        .document_id = doc_id,
        .page_index = state.page_index + 1,
    });
}

void State::getLayout(
    OnePageContinuous& page_state,
    ViewState& state,
    view::IRect rect,
    std::vector<view::Placement>& layout) {
    auto doc_id = *getDocumentId();
    auto page_count = doc_storage_->getDocument(doc_id).pageCount();

    fixRowOffset(page_state.row_offset, rect.height, 1, state);

    rect = view::horizontal(
               view::constraints::Fraction{0.10f / state.zoom},
               view::constraints::Fill{1},
               view::constraints::Fraction{0.10f / state.zoom})
               .split(rect)[1];

    auto top_height = rect.height - page_state.row_offset;
    auto top = offsetRect(rect, 0, -page_state.row_offset);
    auto bottom = offsetRect(rect, 0, +top_height);

    layout.push_back({
        .position =
            {
                .rect = top,
                .hor = view::AxisFit::Center,
                .vert = view::AxisFit::Max,
            },
        .document_id = doc_id,
        .page_index = state.page_index,
    });

    if (state.page_index + 1 >= page_count) {
        return;
    }

    layout.push_back({
        .position =
            {
                .rect = bottom,
                .hor = view::AxisFit::Center,
                .vert = view::AxisFit::Min,
            },
        .document_id = doc_id,
        .page_index = state.page_index + 1,
    });
}

void State::getLayout(
    TwoPagesContinuous& page_state,
    ViewState& state,
    view::IRect rect,
    std::vector<view::Placement>& layout) {
    auto doc_id = *getDocumentId();
    auto page_count = doc_storage_->getDocument(doc_id).pageCount();

    fixRowOffset(page_state.row_offset, rect.height, 2, state);

    auto rects =
        view::horizontal(
            view::constraints::Fraction{0.05f / state.zoom},
            view::constraints::Fill{1},  // rects[1]
            view::constraints::Fill{1},  // rects[2]
            view::constraints::Fraction{0.05f / state.zoom})
            .split(rect);

    uint32_t top_height = rect.height - page_state.row_offset;
    auto top_left = offsetRect(rects[1], 0, -page_state.row_offset);
    auto top_right = offsetRect(rects[2], 0, -page_state.row_offset);
    auto bottom_left = offsetRect(rects[1], 0, +top_height);
    auto bottom_right = offsetRect(rects[2], 0, +top_height);

    layout.push_back({
        .position =
            {
                .rect = top_left,
                .hor = view::AxisFit::Max,
                .vert = view::AxisFit::Max,
            },
        .document_id = doc_id,
        .page_index = state.page_index,
    });

    if (state.page_index + 1 >= page_count) {
        return;
    }

    layout.push_back({
        .position =
            {
                .rect = top_right,
                .hor = view::AxisFit::Min,
                .vert = view::AxisFit::Max,
            },
        .document_id = doc_id,
        .page_index = state.page_index + 1,
    });

    if (state.page_index + 2 >= page_count) {
        return;
    }

    layout.push_back({
        .position =
            {
                .rect = bottom_left,
                .hor = view::AxisFit::Max,
                .vert = view::AxisFit::Min,
            },
        .document_id = doc_id,
        .page_index = state.page_index + 2,
    });

    if (state.page_index + 3 >= page_count) {
        return;
    }

    layout.push_back({
        .position =
            {
                .rect = bottom_right,
                .hor = view::AxisFit::Min,
                .vert = view::AxisFit::Min,
            },
        .document_id = doc_id,
        .page_index = state.page_index + 3,
    });
}

void State::fixRowOffset(int32_t& row_offset, uint32_t height, uint32_t delta, ViewState& state) {
    core::log::global()->info(
        "fixRowOffset before: row_offset={}, page_index={}", row_offset, state.page_index);

    // Fix row_offset when we know the rectangle
    if (row_offset < 0) {
        int count = 1 + -row_offset / int(height);
        row_offset += count * height;
        nextPage(-delta * count, state);
    }

    if (row_offset >= height) {
        int count = row_offset / int(height);
        row_offset -= count * height;
        nextPage(delta * count, state);
    }

    core::log::global()->info(
        "fixRowOffset after: row_offset={}, page_index={}", row_offset, state.page_index);
}

void State::addDocument(DocumentId id) {
    ids_.push_back(id);

    if (!idx_) {
        idx_ = 0;
        return;
    }
}

void State::onDocumentChanged(DocumentId from, DocumentId to) {
    auto it = std::ranges::find(ids_, from);
    *it = to;

    auto& state = state_[to] = state_[from];
    state_.erase(from);

    auto page_count = doc_storage_->getDocument(to).pageCount();
    if (state.page_index >= page_count) {
        state.page_index = page_count - 1;
    }
}

void State::scrollNext(int times) {
    if (auto state = getState()) {
        utils::visit(state->page_state, [&](auto& page_state) {
            scrollNext(page_state, times, *state);
        });
    }
}

void State::scrollNext(OnePage, int times, ViewState& state) {
    nextPage(times, state);
}

void State::scrollNext(TwoPages, int times, ViewState& state) {
    nextPage(2 * times, state);
}

void State::scrollNext(OnePageContinuous& page_state, int times, ViewState& state) {
    page_state.row_offset += 10 * times;
}

void State::scrollNext(TwoPagesContinuous& page_state, int times, ViewState& state) {
    page_state.row_offset += 10 * times;
}

void State::setPageIndex(uint32_t index) {
    if (auto state = getState()) {
        auto page_count = doc_storage_->getDocument(*getDocumentId()).pageCount();
        state->page_index = std::min(page_count - 1, int(index));

        utils::visit(
            state->page_state,
            [](auto&&) {
            },
            [](OnePageContinuous& p) {
                p.row_offset = 0;
            },
            [](TwoPagesContinuous& p) {
                p.row_offset = 0;
            });
    }
}

void State::nextPage(int delta) {
    if (auto state = getState()) {
        nextPage(delta, *state);
    }
}

void State::nextPage(int delta, ViewState& state) {
    auto doc_id = *getDocumentId();
    auto page_count = doc_storage_->getDocument(doc_id).pageCount();

    if (delta >= 0) {
        if (state.page_index + delta < page_count) {
            state.page_index += delta;
        } else {
            state.page_index = 0;
        }
    } else {
        delta = -delta;

        if (state.page_index >= delta) {
            state.page_index -= delta;
        } else {
            state.page_index = page_count - 1;
        }
    }
}

void State::nextDocument(int delta) {
    if (idx_) {
        auto& idx = *idx_;
        int index = idx + delta;

        if (index < 0) {
            index = ids_.size() - 1 - ((-index - 1) % ids_.size());
        } else if (index >= ids_.size()) {
            index = index % ids_.size();
        }

        idx = index;
    }
}

void State::nextLayout(int delta) {
    // XXX TODO: support delta other than |delta| = 1
    PDFT_VERIFY(std::abs(delta) == 1);

    if (auto state = getState()) {
        state->page_state = utils::visit(
            state->page_state,
            [&](OnePage) -> PageState {
                return TwoPages{};
            },
            [&](TwoPages) -> PageState {
                return OnePageContinuous{};
            },
            [&](OnePageContinuous state) -> PageState {
                return TwoPagesContinuous{.row_offset = state.row_offset};
            },
            [&](TwoPagesContinuous) -> PageState {
                return OnePage{};
            });
    }
}

State::ViewState* State::getState() {
    if (auto doc_id = getDocumentId()) {
        return &state_[*doc_id];
    }

    return nullptr;
}

std::optional<DocumentId> State::getDocumentId() const {
    return idx_.transform([this](auto idx) {
        return ids_[idx];
    });
}

void State::zoomIn(float coeff) {
    if (auto state = getState()) {
        state->zoom *= coeff;
    }
}

}  // namespace pdft::state::app
