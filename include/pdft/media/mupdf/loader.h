#pragma once

#include "pdft/media/loader.h"

#include "pdft/core/module/module.h"

namespace pdft::media {

class MupdfLoader : public Loader, public Module {
 public:
    Result<Arc<media::Document>> load(const std::filesystem::path& path) const;
};

}  // namespace pdft::media
