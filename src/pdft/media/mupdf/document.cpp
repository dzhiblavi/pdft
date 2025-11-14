#include "pdft/media/mupdf/document.h"
#include "pdft/media/mupdf/context.h"
#include "pdft/media/mupdf/page.h"

#include "pdft/core/log/common_adapters.h"  // IWYU pragma: keep

#include "platform/core/assert.h"

#include "fz_assert.h"

extern "C" {
#include <mupdf/fitz/archive.h>
#include <mupdf/fitz/document.h>
}

namespace pdft::media {

MupdfDocument::~MupdfDocument() noexcept {
    fz_drop_document(thisThreadMupdfContext().native(), document_);
}

MupdfDocument::MupdfDocument(MupdfDocument&& rhs) noexcept
    : path_(std::move(rhs.path_))
    , document_(std::exchange(rhs.document_, nullptr)) {}

size_t MupdfDocument::getPageCount() const {
    auto ctx = thisThreadMupdfContext().native();
    auto page_count = tryFz(ctx, [&] { return fz_count_pages(ctx, document_); });
    platform::verify(page_count, "failed to retrieve document's page count");
    return *page_count;
}

platform::Arc<media::Page> MupdfDocument::getPage(int index) const {
    platform::verify(
        index >= 0 && index < static_cast<int>(getPageCount()),
        "page index {} is out of range",
        index);

    auto ctx = thisThreadMupdfContext().native();
    auto page = tryFz(ctx, [&] { return fz_load_page(ctx, document_, index); });
    platform::verify(page, "failed to retrieve document's page with index {}", index);

    return platform::Arc<media::Page>(new MupdfPage(index, *page));
}

const std::filesystem::path& MupdfDocument::getPath() const {
    return path_;
}

platform::Result<MupdfDocument> MupdfDocument::open(const std::filesystem::path& path) {
    auto ctx = thisThreadMupdfContext().native();
    auto doc = tryFz(ctx, [&] { return fz_open_document(ctx, path.c_str()); });

    if (!doc) {
        return result::makeError(std::format("failed to open document by path {}", path.c_str()));
    }
    return MupdfDocument(path, *doc);
}

}  // namespace pdft::media
