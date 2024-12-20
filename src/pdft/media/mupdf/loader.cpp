#include "pdft/media/mupdf/loader.h"
#include "pdft/media/mupdf/document.h"

#include "pdft/core/result.h"
#include "pdft/core/result/coro/result.h"
#include "pdft/runtime/module/register.h"

namespace pdft::media {

Result<Arc<Document>> MupdfLoader::load(const std::filesystem::path& path) const {
    auto document = co_await MupdfDocument::open(path);
    co_return result::ok(std::make_shared<MupdfDocument>(std::move(document)));
}

}  // namespace pdft::media

PDFT_REGISTER_MODULE(pdft::media::MupdfLoader);
