#pragma once

#include "pdft/media/loader.h"
#include "pdft/media/mupdf/context.h"

namespace pdft::media {

class MupdfLoader : public Loader {
 public:
    explicit MupdfLoader(MupdfContext& context) : context_(context) {}

    Result<Arc<media::Document>> load(const std::filesystem::path& path) const;

 private:
    MupdfContext& context_;
};

}  // namespace pdft::media
