#pragma once

#include "platform/core/result.h"
#include "pdft/media/document.h"

namespace pdft::media {

class Loader {
 public:
    virtual ~Loader() = default;

    virtual platform::Result<platform::Arc<Document>> load(const std::filesystem::path& path) const = 0;
};

}  // namespace pdft::media
