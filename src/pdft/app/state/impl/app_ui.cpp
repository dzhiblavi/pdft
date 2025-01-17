#include "pdft/app/state/impl/app_ui.h"

#include "platform/core/assert.h"
#include "platform/module/register.h"

#include <magic_enum.hpp>

#include <algorithm>

namespace pdft::app {

void AppUI::applyScrollOffset(DocumentModel& m, int pages_per_height) {
    if (m.scroll_offset < 0) {
        int count = 1 + -m.scroll_offset / ScrollPerPage;
        m.scroll_offset += count * ScrollPerPage;
        nextPage(-pages_per_height * count);
    }

    if (m.scroll_offset >= ScrollPerPage) {
        int count = m.scroll_offset / ScrollPerPage;
        m.scroll_offset -= count * ScrollPerPage;
        nextPage(pages_per_height * count);
    }
}

void AppUI::scrollNext(DocumentModel& m, int32_t rows) {
    switch (m.view_mode) {
        using enum ViewMode;

        case OnePage:
            nextPage(rows > 0 ? 1 : -1);
            break;

        case TwoPages:
            nextPage(rows > 0 ? 2 : -2);
            break;

        case OnePageContinuous:
            m.scroll_offset += rows;
            applyScrollOffset(m, 1);
            break;

        case TwoPagesContinuous:
            m.scroll_offset += rows;
            applyScrollOffset(m, 2);
            break;
    }
}

void AppUI::nextPage(DocumentModel& m, int32_t delta) {
    if (delta >= 0) {
        if (m.page_index + delta < m.page_count) {
            m.page_index += delta;
        } else {
            m.page_index = 0;
        }
    } else {
        delta = -delta;

        if (m.page_index >= static_cast<uint32_t>(delta)) {
            m.page_index -= delta;
        } else {
            m.page_index = m.page_count - 1;
        }
    }
}

void AppUI::setPageIndex(DocumentModel& m, uint32_t index) {
    m.scroll_offset = 0;
    m.page_index = std::min(m.page_count - 1, index);
}

void AppUI::nextLayout(DocumentModel& m, int32_t delta) {
    auto current_layout_index = static_cast<int>(m.view_mode);
    auto new_layout_index = (current_layout_index + delta) % magic_enum::enum_count<ViewMode>();

    m.scroll_offset = 0;
    m.view_mode = static_cast<ViewMode>(new_layout_index);
}

void AppUI::setDocument(DocumentModel& m, DocumentId id, uint32_t page_count) {
    m.document_id = id;
    m.page_count = page_count;

    if (m.page_index >= page_count) {
        m.scroll_offset = 0;
        m.page_index = page_count - 1;
    }
}

void AppUI::onInput(char key) {
    model_.input.push_back(key);
}

void AppUI::resetInput() {
    model_.input.clear();
}

void AppUI::addDocument(DocumentId id, uint32_t page_count) {
    model_.documents.emplace_back(id, page_count);
    model_.index = model_.index.value_or(0);
}

void AppUI::removeDocument(DocumentId id) {
    auto& idx = model_.index;
    auto& states = model_.documents;
    platform::verify{idx};

    auto it = std::ranges::find(states, id, &DocumentModel::document_id);
    platform::verify(it != states.end());

    size_t index = it - states.begin();

    if (index < *idx) {
        *idx -= 1;
    } else if (index == *idx && states.size() >= 2) {
        *idx = std::min(states.size() - 2, *idx);
    }

    states.erase(it);

    if (states.empty()) {
        idx = std::nullopt;
    }
}

void AppUI::onDocumentChanged(DocumentId from, DocumentId to, uint32_t page_count) {
    auto it = std::ranges::find(model_.documents, from, &DocumentModel::document_id);
    platform::verify(
        it != model_.documents.end(), "failed to find the old document with id={}", from);
    setDocument(*it, to, page_count);
}

void AppUI::scrollNext(int rows) {
    if (auto state = getDocument()) {
        scrollNext(*state, rows);
    }
}

void AppUI::nextPage(int delta) {
    if (auto state = getDocument()) {
        nextPage(*state, delta);
    }
}

void AppUI::setPageIndex(uint32_t index) {
    if (auto state = getDocument()) {
        setPageIndex(*state, index);
    }
}

void AppUI::nextDocument(int delta) {
    auto& idx = model_.index;

    if (idx) {
        const int n = static_cast<int>(model_.documents.size());
        auto& i = *idx;
        int index = static_cast<int>(i) + delta;

        if (index < 0) {
            index = n - 1 - ((-index - 1) % n);
        } else if (index >= static_cast<int>(n)) {
            index = index % n;
        }

        idx = index;
    }
}

void AppUI::nextLayout(int delta) {
    if (auto state = getDocument()) {
        nextLayout(*state, delta);
    }
}

void AppUI::invertColors() {
    model_.display.invert_colors = !model_.display.invert_colors;
}

void AppUI::updateScalingFactor(float coeff) {
    model_.display.scaling_coeff *= coeff;
}

void AppUI::toggleSidebar() {
    model_.sidebar = !model_.sidebar;
}

void AppUI::toggleBold() {
    model_.display.make_bolder = !model_.display.make_bolder;
}

void AppUI::setPalette(const std::optional<std::string>& name) {
    model_.display.palette = name;
}

platform::Maybe<DocumentId> AppUI::getDocumentId() const {
    return model_.index.transform([this](auto i) { return model_.documents[i].document_id; });
}

DocumentModel* AppUI::getDocument() {
    auto& idx = model_.index;

    if (idx) {
        return &model_.documents[*idx];
    }

    return nullptr;
}

const DocumentModel* AppUI::getDocument() const {
    auto& idx = model_.index;

    if (idx) {
        return &model_.documents[*idx];
    }

    return nullptr;
}

const UIModel& AppUI::getModel() const {
    return model_;
}

UIModel& AppUI::getModel() {
    return model_;
}

}  // namespace pdft::app

PLATFORM_REGISTER_MODULE(pdft::app::AppUI);
