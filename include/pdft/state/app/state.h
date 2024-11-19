#include "pdft/state/document_storage.h"
#include "pdft/state/state.h"

#include <optional>
#include <unordered_map>
#include <variant>
#include <vector>

namespace pdft::state::app {

class State : public state::State {
 public:
    explicit State(std::shared_ptr<DocumentStorage> doc_storage)
        : doc_storage_(std::move(doc_storage)) {}

    void getLayout(view::IRect rect, std::vector<view::Placement>& layout) override;
    void addDocument(DocumentId id) override;
    void onDocumentChanged(DocumentId from, DocumentId to) override;
    void scrollNext(int delta) override;
    void nextPage(int delta) override;
    void setPageIndex(uint32_t index) override;
    void nextDocument(int delta) override;
    void nextLayout(int delta) override;
    void zoomIn(float coeff) override;

 private:
    struct OnePage {};
    struct TwoPages {};
    struct OnePageContinuous {
        int32_t row_offset;
    };
    struct TwoPagesContinuous {
        int32_t row_offset;
    };
    using PageState = std::variant<OnePage, TwoPages, OnePageContinuous, TwoPagesContinuous>;

    struct ViewState {
        PageState page_state = OnePage{};
        uint32_t page_index = 0;
        float zoom = 1.0f;
    };

    void scrollNext(OnePage, int times, ViewState& state);
    void scrollNext(TwoPages, int times, ViewState& state);
    void scrollNext(OnePageContinuous&, int times, ViewState& state);
    void scrollNext(TwoPagesContinuous&, int times, ViewState& state);

    void nextPage(int delta, ViewState& state);

    void getLayout(OnePage, ViewState&, view::IRect, std::vector<view::Placement>& layout);
    void getLayout(TwoPages, ViewState&, view::IRect, std::vector<view::Placement>& layout);
    void getLayout(
        OnePageContinuous&, ViewState&, view::IRect, std::vector<view::Placement>& layout);
    void getLayout(
        TwoPagesContinuous&, ViewState&, view::IRect, std::vector<view::Placement>& layout);

    void fixRowOffset(int32_t& row_offset, uint32_t height, uint32_t delta, ViewState& state);

    ViewState* getState();
    std::optional<DocumentId> getDocumentId() const;

    std::shared_ptr<DocumentStorage> doc_storage_;
    std::optional<size_t> idx_ = std::nullopt;
    std::vector<DocumentId> ids_;
    std::unordered_map<DocumentId, ViewState> state_;
};

}  // namespace pdft::state::app
