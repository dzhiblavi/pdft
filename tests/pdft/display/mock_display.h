#pragma once

#include "pdft/display/display.h"

#include <gmock/gmock.h>

namespace pdft::display::mock {

class Display : public display::Display {
 public:
    MOCK_METHOD(
        asio::awaitable<void>,
        loadImage,
        (display::ImageId id, const display::Image& image, core::io::OutBuffer& buffer),
        (override));

    MOCK_METHOD(
        asio::awaitable<void>,
        displayImage,
        (display::ImageId id,
         const view::IRect& rect,
         const view::FRect& image_subarea,
         core::io::OutBuffer& buffer),
        (override));

    MOCK_METHOD(void, clear, (core::io::OutBuffer & buffer), (override));
    MOCK_METHOD(void, unload, (display::ImageId id, core::io::OutBuffer& buffer), (override));
    MOCK_METHOD(void, unloadAll, (core::io::OutBuffer & buffer), (override));
    MOCK_METHOD(bool, isLoaded, (display::ImageId id), (const, override));
};

}  // namespace pdft::display::mock
