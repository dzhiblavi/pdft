#pragma once

#include "pdft/state/document_storage.h"
#include "pdft/tui/context.h"
#include "pdft/tui/layout/rect.h"

#include <asio/awaitable.hpp>

namespace pdft::state {

class State {
 public:
    virtual ~State() = default;

    virtual asio::awaitable<void> render(tui::layout::IRect rect, tui::Context& ctx) = 0;

    virtual void onInput(char key) = 0;
    virtual void resetInput() = 0;

    virtual void addDocument(DocumentId id) = 0;
    virtual void onDocumentChanged(DocumentId from, DocumentId to) = 0;

    virtual void scrollNext(int times) = 0;
    virtual void nextPage(int delta) = 0;
    virtual void setPageIndex(uint32_t index) = 0;
    virtual void nextDocument(int delta) = 0;
    virtual void nextLayout(int delta) = 0;
    virtual void updateZoom(float coeff) = 0;
    virtual void invertColors() = 0;
    virtual void updateScalingFactor(float coeff) = 0;

    virtual std::optional<DocumentId> getCurrentDocumentId() const = 0;
};

}  // namespace pdft::state
