#include "pdft/view/app/state.h"
#include "pdft/core/log/log.h"
#include "pdft/utils/visit.h"
#include "pdft/view/layout.h"

namespace pdft::view::app {

void State::getLayout(IRect rect, std::vector<Placement>& layout) {
    layout.clear();

    if (auto* state = getState()) {
        utils::visit(state->page_state, [&](auto& page_state) {
            getLayout(page_state, *state, rect, layout);
        });
    }
}

void State::getLayout(OnePage, ViewState& state, IRect rect, std::vector<Placement>& layout) {
    auto doc_id = *getDocumentId();
    auto page_count = doc_storage_->getDocument(doc_id).pageCount();

    layout.push_back({
        .position =
            {
                .rect = rect,
                .fit = display::FitStrategy::Center,
            },
        .document_id = doc_id,
        .page_index = state.page_index,
    });
}

void State::getLayout(TwoPages, ViewState& state, IRect rect, std::vector<Placement>& layout) {
    auto doc_id = *getDocumentId();
    auto page_count = doc_storage_->getDocument(doc_id).pageCount();

    auto rects =
        horizontal(
            constraints::Fraction{0.05f / state.zoom},
            constraints::Fill{1},
            constraints::Length{1},
            constraints::Fill{1},
            constraints::Fraction{0.05f / state.zoom})
            .split(rect);

    layout.push_back({
        .position =
            {
                .rect = rects[1],
                .fit = display::FitStrategy::Right,
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
                .fit = display::FitStrategy::Left,
            },
        .document_id = doc_id,
        .page_index = state.page_index + 1,
    });
}

void State::getLayout(
    OnePageContinuous& page_state, ViewState& state, IRect rect, std::vector<Placement>& layout) {
    auto doc_id = *getDocumentId();
    auto page_count = doc_storage_->getDocument(doc_id).pageCount();

    fixRowOffset(page_state.row_offset, rect.height, 1, state);

    rect = horizontal(
               constraints::Fraction{0.10f / state.zoom},
               constraints::Fill{1},
               constraints::Fraction{0.10f / state.zoom})
               .split(rect)[1];

    uint32_t top_height = rect.height - page_state.row_offset;
    auto rects = vertical(constraints::Length{top_height}, constraints::Fill{1}).split(rect);

    layout.push_back({
        .position =
            {
                .rect = rects[0],
                .fit = display::FitStrategy::ClipRight,
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
                .rect = rects[1],
                .fit = display::FitStrategy::ClipLeft,
            },
        .document_id = doc_id,
        .page_index = state.page_index + 1,
    });
}

void State::getLayout(
    TwoPagesContinuous& page_state, ViewState& state, IRect rect, std::vector<Placement>& layout) {
    auto doc_id = *getDocumentId();
    auto page_count = doc_storage_->getDocument(doc_id).pageCount();

    fixRowOffset(page_state.row_offset, rect.height, 2, state);

    auto rects =
        horizontal(
            constraints::Fraction{0.05f / state.zoom},
            constraints::Fill{1},
            constraints::Fill{1},
            constraints::Fraction{0.05f / state.zoom})
            .split(rect);

    uint32_t top_height = rect.height - page_state.row_offset;
    auto lrects = vertical(constraints::Length{top_height}, constraints::Fill{1}).split(rects[1]);
    auto rrects = vertical(constraints::Length{top_height}, constraints::Fill{1}).split(rects[2]);

    layout.push_back({
        .position =
            {
                .rect = lrects[0],
                .fit = display::FitStrategy::ClipRight,
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
                .rect = rrects[0],
                .fit = display::FitStrategy::ClipRight,
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
                .rect = lrects[1],
                .fit = display::FitStrategy::ClipLeft,
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
                .rect = rrects[1],
                .fit = display::FitStrategy::ClipLeft,
            },
        .document_id = doc_id,
        .page_index = state.page_index + 3,
    });
}

void State::fixRowOffset(int32_t& row_offset, uint32_t height, uint32_t delta, ViewState& state) {
    core::log::global()->info(
        "fixRowOffset before: row_offset={}, page_index={}", row_offset, state.page_index);

    // Fix row_offset when we know the rectangle
    while (row_offset < 0) {
        row_offset += height;
        nextPage(-delta, state);
    }

    while (row_offset >= height) {
        row_offset -= height;
        nextPage(delta, state);
    }

    core::log::global()->info(
        "fixRowOffset after: row_offset={}, page_index={}", row_offset, state.page_index);
}

void State::addDocument(media::pdf::DocumentId id) {
    ids_.push_back(id);

    if (!idx_) {
        idx_ = 0;
        return;
    }
}

void State::onDocumentChanged(media::pdf::DocumentId from, media::pdf::DocumentId to) {
    auto it = std::ranges::find(ids_, from);
    *it = to;

    auto& state = state_[to] = state_[from];
    state_.erase(from);

    auto page_count = doc_storage_->getDocument(to).pageCount();
    if (state.page_index >= page_count) {
        state.page_index = page_count - 1;
    }
}

void State::scrollNext() {
    if (auto state = getState()) {
        utils::visit(state->page_state, [&](auto& page_state) {
            scrollNext(page_state, *state);
        });
    }
}

void State::scrollNext(OnePage, ViewState& state) {
    nextPage(1, state);
}

void State::scrollNext(TwoPages, ViewState& state) {
    nextPage(2, state);
}

void State::scrollNext(OnePageContinuous& page_state, ViewState& state) {
    page_state.row_offset += 10;
}

void State::scrollNext(TwoPagesContinuous& page_state, ViewState& state) {
    page_state.row_offset += 10;
}

void State::scrollPrev() {
    if (auto state = getState()) {
        utils::visit(state->page_state, [&](auto& page_state) {
            scrollPrev(page_state, *state);
        });
    }
}

void State::scrollPrev(OnePage, ViewState& state) {
    nextPage(-1, state);
}

void State::scrollPrev(TwoPages, ViewState& state) {
    nextPage(-2, state);
}

void State::scrollPrev(OnePageContinuous& page_state, ViewState& state) {
    page_state.row_offset -= 10;
}

void State::scrollPrev(TwoPagesContinuous& page_state, ViewState& state) {
    page_state.row_offset -= 10;
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

void State::nextDocument() {
    if (idx_) {
        auto& idx = *idx_;

        if (idx + 1 < ids_.size()) {
            ++idx;
        } else {
            idx = 0;
        }
    }
}

void State::prevDocument() {
    if (idx_) {
        auto& idx = *idx_;

        if (idx > 0) {
            --idx;
        } else {
            idx = ids_.size() - 1;
        }
    }
}

void State::nextLayout() {
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

void State::prevLayout() {
    if (auto state = getState()) {
        state->page_state = utils::visit(
            state->page_state,
            [&](OnePage) -> PageState {
                return TwoPagesContinuous{.row_offset = 0};
            },
            [&](TwoPages) -> PageState {
                return OnePage{};
            },
            [&](OnePageContinuous) -> PageState {
                return TwoPages{};
            },
            [&](TwoPagesContinuous state) -> PageState {
                return OnePageContinuous{.row_offset = state.row_offset};
            });
    }
}

State::ViewState* State::getState() {
    if (auto doc_id = getDocumentId()) {
        return &state_[*doc_id];
    }

    return nullptr;
}

std::optional<State::DocumentId> State::getDocumentId() const {
    if (const auto& idx = idx_) {
        return ids_[*idx];
    }

    return std::nullopt;
}

void State::zoomIn() {
    if (auto state = getState()) {
        state->zoom *= 1.05;
    }
}

void State::zoomOut() {
    if (auto state = getState()) {
        state->zoom /= 1.05;
    }
}

}  // namespace pdft::view::app
