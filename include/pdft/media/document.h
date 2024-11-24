#pragma once

#include "pdft/media/page.h"

#include <filesystem>

namespace pdft::media {

class Document {
 public:
    virtual ~Document() = default;

    virtual size_t getPageCount() const = 0;
    virtual std::unique_ptr<Page> getPage(int index) const = 0;
    virtual const std::filesystem::path& getPath() const = 0;
};

}  // namespace pdft::media
