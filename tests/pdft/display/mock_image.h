#pragma once

#include "pdft/display/image.h"

#include <gmock/gmock.h>

namespace pdft::display::mock {

class Image : public display::Image {
 public:
    MOCK_METHOD(ImageSize, size, (), (const, override));
    MOCK_METHOD(std::string_view, samples, (), (const, override));
};

}  // namespace pdft::display::mock
