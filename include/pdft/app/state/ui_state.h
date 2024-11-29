#pragma once

#include "pdft/app/state/document_storage.h"

#include <vector>

namespace pdft::app::state {

enum class ViewMode {
    OnePage = 0,
    TwoPages = 1,
    OnePageContinuous = 2,
    TwoPagesContinuous = 3,
};

struct DocumentState {
    void scrollNext(int32_t delta);
    void nextPage(int32_t delta);
    void setPageIndex(uint32_t index);
    void nextLayout(int32_t delta);
    void setDocument(DocumentId id, uint32_t page_count);

    DocumentId document_id;
    uint32_t page_count;
    uint32_t page_index = 0;
    int32_t scroll_offset = 0;
    ViewMode view_mode = ViewMode::OnePage;

 private:
    void applyScrollOffset(uint32_t pages_per_page);
};

struct UIState {
 public:
    void onInput(char key);
    void resetInput();

    void addDocument(DocumentId id, uint32_t page_count);
    void onDocumentChanged(DocumentId from, DocumentId to, uint32_t page_count);

    void scrollNext(int times);
    void nextPage(int delta);
    void setPageIndex(uint32_t index);
    void nextDocument(int delta);
    void nextLayout(int delta);
    void invertColors();
    void updateScalingFactor(float coeff);
    void toggleSidebar();

    std::optional<DocumentId> getCurrentDocumentId() const;
    const DocumentState* getImmutableState() const;

    std::optional<size_t> idx = std::nullopt;
    std::vector<DocumentState> states;
    std::string current_input;
    bool invert_colors = false;
    float scaling_coeff = 1.f;
    bool show_sidebar = false;

 private:
    DocumentState* getState();
};

}  // namespace pdft::app::state
