#include "pdft/state/state.h"
#include "pdft/core/assert.h"
#include "pdft/core/log/log.h"

#include <magic_enum.hpp>

#include <algorithm>

namespace pdft::state {

void DocumentState::applyScrollOffset(uint32_t pages_per_height) {
    log::debug(
        "applyScrollOffset before: scroll_offset={}, page_index={}", scroll_offset, page_index);

    if (scroll_offset < 0) {
        int count = 1 + -scroll_offset / 100;
        scroll_offset += count * 100;
        nextPage(-pages_per_height * count);
    }

    if (scroll_offset >= 100) {
        int count = scroll_offset / 100;
        scroll_offset -= count * 100;
        nextPage(pages_per_height * count);
    }

    log::debug(
        "applyScrollOffset after: scroll_offset={}, page_index={}", scroll_offset, page_index);
}

void DocumentState::scrollNext(int32_t rows) {
    switch (view_mode) {
        using enum ViewMode;

        case OnePage:
            nextPage(rows > 0 ? 1 : -1);
            break;

        case TwoPages:
            nextPage(rows > 0 ? 2 : -2);
            break;

        case OnePageContinuous:
            scroll_offset += rows;
            applyScrollOffset(1);
            break;

        case TwoPagesContinuous:
            scroll_offset += rows;
            applyScrollOffset(2);
            break;
    }
}

void DocumentState::nextPage(int32_t delta) {
    if (delta >= 0) {
        if (page_index + delta < page_count) {
            page_index += delta;
        } else {
            page_index = 0;
        }
    } else {
        delta = -delta;

        if (page_index >= static_cast<uint32_t>(delta)) {
            page_index -= delta;
        } else {
            page_index = page_count - 1;
        }
    }
}

void DocumentState::setPageIndex(uint32_t index) {
    scroll_offset = 0;
    page_index = std::min(page_count - 1, index);
}

void DocumentState::nextLayout(int32_t delta) {
    auto current_layout_index = static_cast<int>(view_mode);
    auto new_layout_index = (current_layout_index + delta) % magic_enum::enum_count<ViewMode>();

    scroll_offset = 0;
    view_mode = static_cast<ViewMode>(new_layout_index);
}

void DocumentState::setDocument(DocumentId id, uint32_t page_count) {
    document_id = id;
    this->page_count = page_count;

    if (page_index >= page_count) {
        scroll_offset = 0;
        page_index = page_count - 1;
    }
}

void State::onInput(char key) {
    current_input.push_back(key);
}

void State::resetInput() {
    current_input.clear();
}

void State::addDocument(DocumentId id, uint32_t page_count) {
    states.emplace_back(id, page_count);
    idx = idx.value_or(0);
}

void State::onDocumentChanged(DocumentId from, DocumentId to, uint32_t page_count) {
    auto it = std::ranges::find(states, from, &DocumentState::document_id);
    PDFT_VERIFY(it != states.end(), "failed to find the old document with id={}", from);
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
    if (idx) {
        const size_t n = states.size();
        auto& i = *idx;
        int index = i + delta;

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

DocumentState* State::getState() {
    if (idx) {
        return &states[*idx];
    }

    return nullptr;
}

const DocumentState* State::getState() const {
    if (idx) {
        return &states[*idx];
    }

    return nullptr;
}

void State::invertColors() {
    image_properties.invert_colors = !image_properties.invert_colors;
}

void State::updateScalingFactor(float coeff) {
    image_properties.scaling_factor *= coeff;
}

void State::toggleSidebar() {
    show_sidebar = !show_sidebar;
}

std::optional<DocumentId> State::getCurrentDocumentId() const {
    return idx.transform([this](auto i) { return states[i].document_id; });
}

}  // namespace pdft::state
