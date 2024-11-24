#pragma once

#include "pdft/media/page.h"

#include <gmock/gmock.h>

namespace pdft::media::mock {

class Page : public media::Page {
 public:
    MOCK_METHOD(
        std::shared_ptr<display::Image>,
        getImage,
        (const display::ImageProperties& properties),
        (const, override));

    MOCK_METHOD(PageSize, getDefaultSize, (), (const, override));
    MOCK_METHOD(size_t, getPageIndex, (), (const, override));
};

}  // namespace pdft::media::mock
