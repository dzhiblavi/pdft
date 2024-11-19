#pragma once

#include "pdft/core/io/buffer.h"
#include "pdft/state/document_storage.h"
#include "pdft/view/rect.h"

namespace pdft::state {

class State {
 public:
    virtual ~State() = default;

    virtual void render(view::IRect rect, core::io::OutBuffer& buffer) = 0;

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
};

}  // namespace pdft::state
