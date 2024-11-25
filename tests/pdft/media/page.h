#pragma once

#include "pdft/media/page.h"

#include <gmock/gmock.h>

namespace pdft::media::mock {

class Page : public media::Page {
 public:
    MOCK_METHOD(
        std::shared_ptr<image::Image>,
        getImage,
        (const image::ImageProperties& properties),
        (const, override));

    MOCK_METHOD(PageSize, getDefaultSize, (), (const, override));
    MOCK_METHOD(size_t, getPageIndex, (), (const, override));
};

}  // namespace pdft::media::mock
