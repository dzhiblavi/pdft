#pragma once

#include "pdft/core/assert.h"
#include "pdft/tui/context.h"
#include "pdft/tui/display/display.h"
#include "pdft/tui/layout/rect.h"

namespace pdft::tui::image {

struct Image {
    uint32_t id;
    layout::FRect subarea;

    Image& setSubarea(layout::FRect subarea) {
        this->subarea = subarea;
        return *this;
    }

    static Image fromId(uint32_t id) {
        Image image;
        image.id = id;
        return image.setSubarea({
            .x = 0.f,
            .y = 0.f,
            .width = 1.f,
            .height = 1.f,
        });
    }

    static Image make(uint32_t id, layout::FRect subarea) {
        Image image;
        image.id = id;
        return image.setSubarea(subarea);
    }

 private:
    Image() = default;
};

inline asio::awaitable<void> render(
    const Image& image, const layout::IRect& rect, display::Display& display, Context& ctx) {
    PDFT_VERIFY(display.isLoaded(image.id));
    co_await display.displayImage(image.id, rect, image.subarea, ctx);
}

}  // namespace pdft::tui::image
