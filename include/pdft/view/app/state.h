#include "pdft/view/state.h"

#include <optional>
#include <unordered_map>
#include <vector>

namespace pdft::view::app {

class State : public view::State {
 public:
    explicit State(std::shared_ptr<media::pdf::DocumentStorage> doc_storage)
        : doc_storage_(std::move(doc_storage)) {}

    void getLayout(IRect rect, std::vector<Placement>& layout) override;

    void addDocument(media::pdf::DocumentId id) override;
    void onDocumentChanged(media::pdf::DocumentId from, media::pdf::DocumentId to) override;

    void nextPage() override;
    void prevPage() override;

    void nextDocument() override;
    void prevDocument() override;

    void nextLayout() override;
    void prevLayout() override;

 private:
    using DocumentId = media::pdf::DocumentId;

    struct ViewState {
        bool two_pages = false;
        uint32_t page_index = 0;
    };

    ViewState* getState();
    std::optional<DocumentId> getDocumentId() const;

    std::shared_ptr<media::pdf::DocumentStorage> doc_storage_;
    std::optional<size_t> idx_ = std::nullopt;
    std::vector<DocumentId> ids_;
    std::unordered_map<DocumentId, ViewState> state_;
};

}  // namespace pdft::view::app
