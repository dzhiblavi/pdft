#pragma once

#include "pdft/media/loader.h"

namespace pdft::media {

class MupdfLoader : public Loader {
 public:
    Result<Arc<media::Document>> load(const std::filesystem::path& path) const;
};

}  // namespace pdft::media
