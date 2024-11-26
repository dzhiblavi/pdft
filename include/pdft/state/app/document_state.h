#pragma once

#include "pdft/tui/context.h"
#include "pdft/tui/elements.h"
#include "pdft/view/renderer.h"

namespace pdft::state::app {

class DocumentState {
 public:
    DocumentState(DocumentId id, uint32_t page_count, std::shared_ptr<view::Renderer> renderer)
        : document_id_(id)
        , page_count_(page_count)
        , renderer_(std::move(renderer)) {}

    tui::Element render(const image::ImageProperties& properties, tui::Context& ctx);

    void scrollNext(int32_t delta);
    void nextPage(int32_t delta);
    void setPageIndex(uint32_t index);
    void nextLayout(int32_t delta);
    void updateZoom(float coeff);

    DocumentId getDocumentId() const;
    void setDocument(DocumentId id, uint32_t page_count);

    uint32_t getPageIndex() const;
    uint32_t getPageCount() const;

 private:
    enum class ViewMode {
        OnePage = 0,
        TwoPages = 1,
        OnePageContinuous = 2,
        TwoPagesContinuous = 3,
    };

    void applyScrollOffset(uint32_t height, int32_t pages_per_height);
    tui::Element renderOnePage(const image::ImageProperties& properties, tui::Context& ctx);
    tui::Element renderTwoPages(const image::ImageProperties& properties, tui::Context& ctx);
    tui::Element renderOnePageContinuous(
        const image::ImageProperties& properties, tui::Context& ctx);
    tui::Element renderTwoPagesContinuous(
        const image::ImageProperties& properties, tui::Context& ctx);

    DocumentId document_id_;
    uint32_t page_count_;
    std::shared_ptr<view::Renderer> renderer_;
    uint32_t page_index_ = 0;
    int32_t scroll_offset_ = 0;
    float zoom_ = 1.0f;
    ViewMode view_mode_ = ViewMode::OnePage;
    tui::IRect rect_;
};

}  // namespace pdft::state::app
