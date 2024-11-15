#include "pdft/view/app/state.h"
#include "pdft/view/layout.h"

namespace pdft::view::app {

void State::getLayout(IRect rect, std::vector<Placement>& layout) {
    layout.clear();

    if (auto state = getState()) {
        auto doc_id = *getDocumentId();
        auto page_count = doc_storage_->getDocument(doc_id).pageCount();

        if (state->two_pages && state->page_index + 1 < page_count) {
            auto rects =
                horizontal(
                    constraints::Fraction{0.05f},
                    constraints::Fill{1},
                    constraints::Length{1},
                    constraints::Fill{1},
                    constraints::Fraction{0.05f})
                    .split(rect);

            layout.push_back({
                .position =
                    {
                        .rect = rects[1],
                        .fit = display::FitStrategy::Right,
                    },
                .document_id = doc_id,
                .page_index = state->page_index,
            });

            layout.push_back({
                .position =
                    {
                        .rect = rects[3],
                        .fit = display::FitStrategy::Left,
                    },
                .document_id = doc_id,
                .page_index = state->page_index + 1,
            });
        } else {
            layout.push_back({
                .position =
                    {
                        .rect = rect,
                        .fit = display::FitStrategy::Center,
                    },
                .document_id = doc_id,
                .page_index = state->page_index,
            });
        }
    }
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

void State::nextPage() {
    if (auto state = getState()) {
        int delta = state->two_pages ? 2 : 1;
        auto doc_id = *getDocumentId();
        auto page_count = doc_storage_->getDocument(doc_id).pageCount();

        if (state->page_index + delta < page_count) {
            state->page_index += delta;
        } else {
            state->page_index = 0;
        }
    }
}

void State::prevPage() {
    if (auto state = getState()) {
        int delta = state->two_pages ? 2 : 1;
        auto doc_id = *getDocumentId();
        auto page_count = doc_storage_->getDocument(doc_id).pageCount();

        if (state->page_index >= delta) {
            state->page_index -= delta;
        } else {
            state->page_index = page_count - 1;
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
        state->two_pages = !state->two_pages;
    }
}

void State::prevLayout() {
    nextLayout();
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

}  // namespace pdft::view::app
