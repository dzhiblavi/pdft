#include "pdft/state/app/state.h"
#include "pdft/core/assert.h"

#include <algorithm>
#include <format>

namespace pdft::state::app {

tui::Element State::getSidebar() {
    if (!show_sidebar_) {
        return tui::emptyElement();
    }

    std::vector<tui::Element> elements;
    for (size_t i = 0; i < states_.size(); ++i) {
        const auto& state = states_[i];
        bool is_current = i == *idx_;
        auto doc_id = state.getDocumentId();
        const auto& path = doc_storage_->getDocument(doc_id).getPath();
        auto line = std::format("{}{}", is_current ? "* " : "", path.filename().c_str());
        elements.push_back(tui::text(std::move(line)));
    }

    return tui::vbox(std::move(elements))  //
           | tui::border                   //
           | tui::size(tui::WIDTH, tui::EQUAL, 20);
}

tui::Element State::render(tui::Context& ctx) {
    auto doc = [&] {
        if (auto state = getState()) {
            return state->render(image_properties_, ctx);
        }

        return tui::emptyElement();
    }();

    auto dom = tui::hbox({
        getSidebar(),
        doc | tui::flex | tui::borderLight,
    });

    return dom;
}

void State::onInput(char key) {
    current_input_.push_back(key);
}

void State::resetInput() {
    current_input_.clear();
}

void State::addDocument(DocumentId id) {
    auto page_count = doc_storage_->getDocument(id).getPageCount();
    states_.emplace_back(id, page_count, renderer_);
    idx_ = idx_.value_or(0);
}

void State::onDocumentChanged(DocumentId from, DocumentId to) {
    auto page_count = doc_storage_->getDocument(to).getPageCount();
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
    image_properties_.invert_colors = !image_properties_.invert_colors;
}

void State::updateScalingFactor(float coeff) {
    image_properties_.scaling_factor *= coeff;
}

void State::toggleSidebar() {
    show_sidebar_ = !show_sidebar_;
}

std::optional<DocumentId> State::getCurrentDocumentId() const {
    return idx_.transform([this](auto i) { return states_[i].getDocumentId(); });
}

}  // namespace pdft::state::app
