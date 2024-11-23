#include "pdft/state/app/state.h"
#include "pdft/core/assert.h"
#include "pdft/view/layout.h"

#include <algorithm>
#include <format>

namespace pdft::state::app {

asio::awaitable<void> State::render(view::IRect rect, core::io::OutBuffer& buffer) {
    namespace constr = view::constraints;

    auto rects = view::vertical(constr::Length(1), constr::Fill(1), constr::Length(1)).split(rect);
    auto info_rects = view::horizontal(constr::Fill(1), constr::Fill(1)).split(rects[2]);

    renderer_->render(
        buffer,
        info_rects[0],
        view::TextPlacement{
            .hor = view::AxisFit::Min,
            .vert = view::AxisFit::Center,
            .overflow = view::TextPlacement::OverflowMode::Truncate,
        },
        current_input_);

    if (auto state = getState()) {
        co_await state->render(
            rects[1],
            {
                .scaling_factor = 1.f,
                .invert_colors = invert_colors_,
            },
            buffer);

        renderer_->render(
            buffer,
            info_rects[1],
            view::TextPlacement{
                .hor = view::AxisFit::Max,
                .vert = view::AxisFit::Center,
                .overflow = view::TextPlacement::OverflowMode::Truncate,
            },
            std::format(
                "document {}: {}/{}",
                state->getDocumentId(),
                state->getPageIndex() + 1,
                state->getPageCount())
                .c_str());
    }
}

void State::onInput(char key) {
    current_input_.push_back(key);
}

void State::resetInput() {
    current_input_.clear();
}

void State::addDocument(DocumentId id) {
    auto page_count = doc_storage_->getDocument(id).pageCount();
    states_.emplace_back(id, page_count, renderer_);
    idx_ = idx_.value_or(0);
}

void State::onDocumentChanged(DocumentId from, DocumentId to) {
    auto page_count = doc_storage_->getDocument(to).pageCount();
    auto it = std::ranges::find(states_, from, &DocumentState::getDocumentId);
    PDFT_VERIFY(it != states_.end(), "failed to find the old document with id={}", from);
    it->setDocument(to, page_count);
}

void State::scrollNext(int rows) {
    if (auto state = getState()) {
        state->scrollNext(rows);
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
        } else if (index >= static_cast<int>(n)) {
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

void State::invertColors() {
    invert_colors_ = !invert_colors_;
}

std::optional<DocumentId> State::getCurrentDocumentId() const {
    return idx_.transform([this](auto i) { return states_[i].getDocumentId(); });
}

}  // namespace pdft::state::app
