#pragma once

#include "pdft/app/state/document_storage.h"
#include "pdft/app/view/display_properties.h"

#include <vector>

namespace pdft::app {

enum class ViewMode {
    OnePage = 0,
    TwoPages = 1,
    OnePageContinuous = 2,
    TwoPagesContinuous = 3,
};

struct DocumentModel {
    DocumentId document_id = 0;
    uint32_t page_count = 0;
    uint32_t page_index = 0;
    int32_t scroll_offset = 0;
    ViewMode view_mode = ViewMode::OnePage;
};

struct UIModel {
    std::optional<size_t> index = std::nullopt;
    std::vector<DocumentModel> documents;
    std::string input;
    bool sidebar = false;
    DisplayProperties display;
};

class UI {
 public:
    static inline constexpr int ScrollPerPage = 100;

    virtual ~UI() = default;
    virtual void onInput(char key) = 0;
    virtual void resetInput() = 0;

    virtual void addDocument(DocumentId id, uint32_t page_count) = 0;
    virtual void removeDocument(DocumentId id) = 0;
    virtual void onDocumentChanged(DocumentId from, DocumentId to, uint32_t page_count) = 0;

    virtual void scrollNext(int times) = 0;
    virtual void nextPage(int delta) = 0;
    virtual void setPageIndex(uint32_t index) = 0;
    virtual void nextDocument(int delta) = 0;
    virtual void nextLayout(int delta) = 0;
    virtual void invertColors() = 0;
    virtual void updateScalingFactor(float coeff) = 0;
    virtual void toggleSidebar() = 0;
    virtual void toggleBold() = 0;
    virtual void setPalette(const std::optional<std::string>& name) = 0;

    virtual const UIModel& getModel() const = 0;
    virtual UIModel& getModel() = 0;
    virtual Maybe<DocumentId> getDocumentId() const = 0;
    virtual DocumentModel* getDocument() = 0;
    virtual const DocumentModel* getDocument() const = 0;
};

}  // namespace pdft::app
