#include "pdft/view/state.h"

#include <optional>
#include <unordered_map>
#include <variant>
#include <vector>

namespace pdft::view::app {

class State : public view::State {
 public:
    explicit State(std::shared_ptr<media::pdf::DocumentStorage> doc_storage)
        : doc_storage_(std::move(doc_storage)) {}

    void getLayout(IRect rect, std::vector<Placement>& layout) override;

    void addDocument(media::pdf::DocumentId id) override;
    void onDocumentChanged(media::pdf::DocumentId from, media::pdf::DocumentId to) override;

    void scrollNext() override;
    void scrollPrev() override;

    void nextDocument() override;
    void prevDocument() override;

    void nextLayout() override;
    void prevLayout() override;

    void zoomIn() override;
    void zoomOut() override;

 private:
    using DocumentId = media::pdf::DocumentId;

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

    void scrollNext(OnePage, ViewState& state);
    void scrollNext(TwoPages, ViewState& state);
    void scrollNext(OnePageContinuous&, ViewState& state);
    void scrollNext(TwoPagesContinuous&, ViewState& state);

    void scrollPrev(OnePage, ViewState& state);
    void scrollPrev(TwoPages, ViewState& state);
    void scrollPrev(OnePageContinuous&, ViewState& state);
    void scrollPrev(TwoPagesContinuous&, ViewState& state);

    void nextPage(int delta, ViewState& state);

    void getLayout(OnePage, ViewState&, IRect, std::vector<Placement>& layout);
    void getLayout(TwoPages, ViewState&, IRect, std::vector<Placement>& layout);
    void getLayout(OnePageContinuous&, ViewState&, IRect, std::vector<Placement>& layout);
    void getLayout(TwoPagesContinuous&, ViewState&, IRect, std::vector<Placement>& layout);

    void fixRowOffset(int32_t& row_offset, uint32_t height, uint32_t delta, ViewState& state);

    ViewState* getState();
    std::optional<DocumentId> getDocumentId() const;

    std::shared_ptr<media::pdf::DocumentStorage> doc_storage_;
    std::optional<size_t> idx_ = std::nullopt;
    std::vector<DocumentId> ids_;
    std::unordered_map<DocumentId, ViewState> state_;
};

}  // namespace pdft::view::app
