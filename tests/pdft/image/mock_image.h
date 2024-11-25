#pragma once

#include "pdft/image/image.h"

#include <gmock/gmock.h>

namespace pdft::image::mock {

class Image : public image::Image {
 public:
    MOCK_METHOD(ImageSize, size, (), (const, override));
    MOCK_METHOD(std::string_view, samples, (), (const, override));
};

}  // namespace pdft::image::mock
