#pragma once

#include "pdft/state/document_storage.h"
#include "pdft/view/placement.h"

namespace pdft::state::app {

class DocumentState {
 public:
    DocumentState(DocumentId id, uint32_t page_count) : document_id_(id), page_count_(page_count) {}

    void getLayout(view::IRect rect, std::vector<view::Placement>& layout);
    void scrollNext(int32_t delta);
    void nextPage(int32_t delta);
    void setPageIndex(uint32_t index);
    void nextLayout(int32_t delta);
    void updateZoom(float coeff);

    DocumentId getDocumentId() const;
    void setDocument(DocumentId id, uint32_t page_count);

 private:
    enum class ViewMode {
        OnePage = 0,
        TwoPages = 1,
        OnePageContinuous = 2,
        TwoPagesContinuous = 3,
    };

    void applyScrollOffset(uint32_t height, int32_t pages_per_height);
    void getLayoutOnePage(view::IRect rect, std::vector<view::Placement>& layout);
    void getLayoutTwoPages(view::IRect rect, std::vector<view::Placement>& layout);
    void getLayoutOnePageContinuous(view::IRect rect, std::vector<view::Placement>& layout);
    void getLayoutTwoPagesContinuous(view::IRect rect, std::vector<view::Placement>& layout);

    DocumentId document_id_;
    uint32_t page_count_;
    uint32_t page_index_ = 0;
    int32_t scroll_offset_ = 0;
    float zoom_ = 1.0f;
    ViewMode view_mode_ = ViewMode::OnePage;
};

}  // namespace pdft::state::app
