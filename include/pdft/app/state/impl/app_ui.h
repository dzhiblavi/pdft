#pragma once

#include "pdft/app/state/ui.h"

namespace pdft::app {

class AppUI : public UI, public Module {
 public:
    void onInput(char key) override;
    void resetInput() override;

    void addDocument(DocumentId id, uint32_t page_count) override;
    void removeDocument(DocumentId id) override;
    void onDocumentChanged(DocumentId from, DocumentId to, uint32_t page_count) override;

    void scrollNext(int times) override;
    void nextPage(int delta) override;
    void setPageIndex(uint32_t index) override;
    void nextDocument(int delta) override;
    void nextLayout(int delta) override;
    void invertColors() override;
    void updateScalingFactor(float coeff) override;
    void toggleSidebar() override;
    void toggleBold() override;
    void setPalette(const std::optional<std::string>& name) override;

    const UIModel& getModel() const override;
    UIModel& getModel() override;
    Maybe<DocumentId> getDocumentId() const override;
    DocumentModel* getDocument() override;
    const DocumentModel* getDocument() const override;

 private:
    void setDocument(DocumentModel& m, DocumentId id, uint32_t page_count);
    void scrollNext(DocumentModel& m, int32_t rows);
    void applyScrollOffset(DocumentModel& m, uint32_t pages_per_height);
    void nextLayout(DocumentModel& m, int32_t delta);
    void nextPage(DocumentModel& m, int32_t delta);
    void setPageIndex(DocumentModel& m, uint32_t index);

    UIModel model_;
};

}  // namespace pdft::app
