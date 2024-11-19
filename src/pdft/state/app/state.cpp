#include "pdft/state/app/state.h"
#include "pdft/core/assert.h"

#include <algorithm>

namespace pdft::state::app {

void State::getLayout(view::IRect rect, std::vector<view::Placement>& layout) {
    if (auto state = getState()) {
        state->getLayout(rect, layout);
    }
}

void State::addDocument(DocumentId id) {
    auto page_count = doc_storage_->getDocument(id).pageCount();
    states_.emplace_back(id, page_count);
    idx_ = idx_.value_or(0);
}

void State::onDocumentChanged(DocumentId from, DocumentId to) {
    auto page_count = doc_storage_->getDocument(to).pageCount();
    auto it = std::ranges::find(states_, from, &DocumentState::getDocumentId);
    PDFT_VERIFY(it != states_.end());
    it->setDocument(to, page_count);
}

void State::scrollNext(int times) {
    if (auto state = getState()) {
        state->scrollNext(times);
    }
}

void State::nextPage(int delta) {
    if (auto state = getState()) {
        state->nextPage(delta);
    }
}

void State::setPageIndex(uint32_t index) {
    if (auto state = getState()) {
        state->setPageIndex(index);
    }
}

void State::nextDocument(int delta) {
    if (idx_) {
        const size_t n = states_.size();
        auto& idx = *idx_;
        int index = idx + delta;

        if (index < 0) {
            index = n - 1 - ((-index - 1) % n);
        } else if (index >= n) {
            index = index % n;
        }

        idx = index;
    }
}

void State::nextLayout(int delta) {
    if (auto state = getState()) {
        state->nextLayout(delta);
    }
}

void State::updateZoom(float coeff) {
    if (auto state = getState()) {
        state->updateZoom(coeff);
    }
}

DocumentState* State::getState() {
    if (idx_) {
        return &states_[*idx_];
    }

    return nullptr;
}

}  // namespace pdft::state::app
