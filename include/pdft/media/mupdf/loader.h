#pragma once

#include "pdft/media/loader.h"
#include "pdft/media/mupdf/context.h"

namespace pdft::media::mupdf {

class Loader : public media::Loader {
 public:
    explicit Loader(Context& context) : context_(context) {}

    Result<Arc<media::Document>> load(const std::filesystem::path& path) const;

 private:
    Context& context_;
};

}  // namespace pdft::media::mupdf
