#pragma once

#include "pdft/core/result.h"
#include "pdft/media/document.h"

namespace pdft::media {

class Loader {
 public:
    virtual ~Loader() = default;

    virtual Result<std::shared_ptr<Document>> load(const std::filesystem::path& path) const = 0;
};

}  // namespace pdft::media
