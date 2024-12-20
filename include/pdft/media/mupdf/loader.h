#pragma once

#include "pdft/media/loader.h"

#include "pdft/runtime/module/module.h"

namespace pdft::media {

class MupdfLoader : public Loader, public rt::Module {
 public:
    Result<Arc<media::Document>> load(const std::filesystem::path& path) const override;
};

}  // namespace pdft::media
