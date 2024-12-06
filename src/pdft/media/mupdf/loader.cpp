#include "pdft/media/mupdf/loader.h"
#include "pdft/media/mupdf/document.h"

#include "pdft/core/result.h"
#include "pdft/core/result/coro/result.h"

namespace pdft::media {

Result<Arc<Document>> MupdfLoader::load(const std::filesystem::path& path) const {
    auto document = co_await MupdfDocument::open(path);
    co_return std::make_shared<MupdfDocument>(std::move(document));
}

}  // namespace pdft::media
