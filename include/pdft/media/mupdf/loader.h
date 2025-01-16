#pragma once

#include "pdft/media/loader.h"

#include "platform/module/module.h"

namespace pdft::media {

class MupdfLoader : public Loader, public platform::rt::Module {
 public:
    platform::Result<platform::Arc<media::Document>> load(
        const std::filesystem::path& path) const override;
};

}  // namespace pdft::media
