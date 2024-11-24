#pragma once

#include "pdft/core/terminal/terminal.h"
#include "pdft/display/display.h"
#include "pdft/state/document_storage.h"
#include "pdft/view/renderer.h"

#include <unordered_map>

namespace pdft::view::app {

class Renderer : public view::Renderer {
 public:
    Renderer(
        core::terminal::Dimensions term_size,
        std::shared_ptr<state::DocumentStorage> docs,
        std::shared_ptr<display::Display> display,
        std::shared_ptr<core::terminal::Terminal> terminal);

    asio::awaitable<void> render(
        core::io::OutBuffer& buffer,
        const IRect& rect,
        const PagePlacement& placement,
        const display::ImageProperties& properties,
        const Page& page) override;

    void render(
        core::io::OutBuffer& buffer,
        const IRect& rect,
        const TextPlacement& placement,
        std::string_view text) override;

 private:
    float calcFactor(media::pdf::PageSize page_size, float scaling_factor);

    struct LoadedImage {
        float cell_wh_ratio;
        display::ImageProperties properties;
    };

    const uint32_t image_id_salt_;
    core::terminal::Dimensions term_dims_;
    std::shared_ptr<state::DocumentStorage> docs_;
    std::shared_ptr<display::Display> display_;
    std::shared_ptr<core::terminal::Terminal> terminal_;
    std::unordered_map<display::ImageId, LoadedImage> loaded_;
};

}  // namespace pdft::view::app
