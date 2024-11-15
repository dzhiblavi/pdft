#include "pdft/core/terminal/terminal.h"
#include "pdft/display/display.h"
#include "pdft/media/pdf/document_storage.h"
#include "pdft/view/renderer.h"

namespace pdft::view::app {

class Renderer : public view::Renderer {
 public:
    Renderer(
        std::shared_ptr<media::pdf::DocumentStorage> docs,
        std::shared_ptr<display::Display> display)
        : term_dims_(core::terminal::getTerminalDimensions())
        , docs_(std::move(docs))
        , display_(std::move(display)) {}

    void render(core::OutBuffer& buffer, const std::vector<Placement>& layout) override;

 private:
    void renderOne(core::OutBuffer& buffer, const Placement& layout);
    float calcFactor(media::pdf::PageSize page_size);

    core::terminal::Dimensions term_dims_;
    std::shared_ptr<media::pdf::DocumentStorage> docs_;
    std::shared_ptr<display::Display> display_;
};

}  // namespace pdft::view::app
