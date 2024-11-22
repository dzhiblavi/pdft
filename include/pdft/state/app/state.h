#include "pdft/state/app/document_state.h"
#include "pdft/state/document_storage.h"
#include "pdft/state/state.h"

#include <optional>
#include <vector>

namespace pdft::state::app {

class State : public state::State {
 public:
    State(std::shared_ptr<DocumentStorage> doc_storage, std::shared_ptr<view::Renderer> renderer)
        : doc_storage_(std::move(doc_storage))
        , renderer_(std::move(renderer)) {}

    asio::awaitable<void> render(view::IRect rect, core::io::OutBuffer& buffer) override;

    void onInput(char key) override;
    void resetInput() override;
    void addDocument(DocumentId id) override;
    void onDocumentChanged(DocumentId from, DocumentId to) override;
    void scrollNext(int delta) override;
    void nextPage(int delta) override;
    void setPageIndex(uint32_t index) override;
    void nextDocument(int delta) override;
    void nextLayout(int delta) override;
    void updateZoom(float coeff) override;

 private:
    DocumentState* getState();

    std::shared_ptr<DocumentStorage> doc_storage_;
    std::shared_ptr<view::Renderer> renderer_;
    std::optional<size_t> idx_ = std::nullopt;
    std::vector<DocumentState> states_;
    std::string current_input_;
};

}  // namespace pdft::state::app
