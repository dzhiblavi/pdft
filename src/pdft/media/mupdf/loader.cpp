#include "pdft/media/mupdf/loader.h"
#include "pdft/media/mupdf/document.h"

#include "platform/core/result.h"
#include "platform/module/register.h"

namespace pdft::media {

platform::Result<platform::Arc<Document>> MupdfLoader::load(
    const std::filesystem::path& path) const {
    auto document = co_await MupdfDocument::open(path);
    co_return platform::Arc<Document>(std::make_shared<MupdfDocument>(std::move(document)));
}

}  // namespace pdft::media

PLATFORM_REGISTER_MODULE(pdft::media::MupdfLoader);
