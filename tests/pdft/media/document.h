#pragma once

#include "pdft/media/document.h"

#include <gmock/gmock.h>

namespace pdft::media::mock {

class Document : public media::Document {
 public:
    MOCK_METHOD(size_t, getPageCount, (), (const, override));
    MOCK_METHOD(std::shared_ptr<Page>, getPage, (int index), (const, override));
    MOCK_METHOD(const std::filesystem::path&, getPath, (), (const, override));
};

}  // namespace pdft::media::mock
