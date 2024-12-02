#include "pdft/media/mupdf/loader.h"
#include "pdft/media/mupdf/document.h"

namespace pdft::media::mupdf {

Result<Arc<media::Document>> Loader::load(const std::filesystem::path& path) const {
    return Document::open(context_, path).transform([](auto document) {
        return std::make_shared<Document>(std::move(document));
    });
}

}  // namespace pdft::media::mupdf
