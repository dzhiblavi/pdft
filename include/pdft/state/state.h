#pragma once

#include "pdft/state/document_storage.h"
#include "pdft/tui/context.h"
#include "pdft/tui/elements.h"

namespace pdft::state {

enum class ViewMode {
    OnePage = 0,
    TwoPages = 1,
    OnePageContinuous = 2,
    TwoPagesContinuous = 3,
};

class State {
 public:
    virtual ~State() = default;

    virtual tui::Element render(tui::Context& ctx) = 0;

    virtual void onInput(char key) = 0;
    virtual void resetInput() = 0;

    virtual void addDocument(DocumentId id) = 0;
    virtual void onDocumentChanged(DocumentId from, DocumentId to) = 0;

    virtual void scrollNext(int times) = 0;
    virtual void nextPage(int delta) = 0;
    virtual void setPageIndex(uint32_t index) = 0;
    virtual void nextDocument(int delta) = 0;
    virtual void nextLayout(int delta) = 0;
    virtual void invertColors() = 0;
    virtual void updateScalingFactor(float coeff) = 0;
    virtual void toggleSidebar() = 0;

    virtual std::optional<DocumentId> getCurrentDocumentId() const = 0;
};

}  // namespace pdft::state
