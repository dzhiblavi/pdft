#pragma once

#include "pdft/media/pdf/document_storage.h"
#include "pdft/view/placement.h"

namespace pdft::view {

class State {
 public:
    virtual ~State() = default;

    virtual void getLayout(IRect rect, std::vector<Placement>& layout) = 0;

    virtual void addDocument(media::pdf::DocumentId id) = 0;
    virtual void onDocumentChanged(media::pdf::DocumentId from, media::pdf::DocumentId to) = 0;

    virtual void nextPage() = 0;
    virtual void prevPage() = 0;

    virtual void nextDocument() = 0;
    virtual void prevDocument() = 0;

    virtual void nextLayout() = 0;
    virtual void prevLayout() = 0;
};

}  // namespace pdft::view
