#include "pdft/core/terminal/terminal.h"
#include "pdft/state/document_storage.h"
#include "pdft/display/display.h"
#include "pdft/view/renderer.h"

#include <unordered_map>

namespace pdft::view::app {

class Renderer : public view::Renderer {
 public:
    Renderer(
        std::shared_ptr<state::DocumentStorage> docs,
        std::shared_ptr<display::Display> display,
        std::shared_ptr<core::terminal::Terminal> terminal)
        : term_dims_(core::terminal::getTerminalDimensions())
        , docs_(std::move(docs))
        , display_(std::move(display))
        , terminal_(std::move(terminal)) {}

    asio::awaitable<void> render(
        core::io::OutBuffer& buffer,
        const IRect& rect,
        const PagePlacement& placement,
        const Page& page) override;

    void render(
        core::io::OutBuffer& buffer,
        const IRect& rect,
        const TextPlacement& placement,
        std::string_view text) override;

 private:
    float calcFactor(media::pdf::PageSize page_size);

    core::terminal::Dimensions term_dims_;
    std::shared_ptr<state::DocumentStorage> docs_;
    std::shared_ptr<display::Display> display_;
    std::shared_ptr<core::terminal::Terminal> terminal_;
    std::unordered_map<media::ImageId, float> image_ratio_;
};

}  // namespace pdft::view::app
