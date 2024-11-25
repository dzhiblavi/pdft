#pragma once

#include "pdft/tui/display/display.h"

#include <gmock/gmock.h>

namespace pdft::tui::display::mock {

class Display : public display::Display {
 public:
    MOCK_METHOD(
        asio::awaitable<void>,
        loadImage,
        (image::ImageId id, const image::Image& image, core::io::OutBuffer& buffer),
        (override));

    MOCK_METHOD(
        asio::awaitable<void>,
        displayImage,
        (image::ImageId id,
         const tui::layout::IRect& rect,
         const tui::layout::FRect& image_subarea,
         core::io::OutBuffer& buffer),
        (override));

    MOCK_METHOD(void, clear, (core::io::OutBuffer & buffer), (override));
    MOCK_METHOD(void, unload, (image::ImageId id, core::io::OutBuffer& buffer), (override));
    MOCK_METHOD(void, unloadAll, (core::io::OutBuffer & buffer), (override));
    MOCK_METHOD(bool, isLoaded, (image::ImageId id), (const, override));
};

}  // namespace pdft::tui::display::mock
