#include "pdft/media/mupdf/document.h"
#include "pdft/core/assert.h"
#include "pdft/media/mupdf/page.h"

#include "fz_assert.h"

extern "C" {
#include <mupdf/fitz/archive.h>
#include <mupdf/fitz/document.h>
}

namespace pdft::media::mupdf {

Document::~Document() noexcept {
    fz_drop_document(context_.native(), document_);
}

Document::Document(Document&& rhs)
    : context_(rhs.context_)
    , path_(std::move(rhs.path_))
    , document_(std::exchange(rhs.document_, nullptr)) {}

size_t Document::getPageCount() const {
    auto ctx = context_.native();
    int page_count = 0;

    PDFT_FZ_VERIFY(page_count = fz_count_pages(ctx, document_));
    return page_count;
}

std::shared_ptr<media::Page> Document::getPage(int index) const {
    PDFT_VERIFY(index >= 0 && index < static_cast<int>(getPageCount()));
    auto ctx = context_.native();
    fz_page* page = nullptr;

    PDFT_FZ_VERIFY(page = fz_load_page(ctx, document_, index));
    PDFT_VERIFY(page);
    return std::shared_ptr<media::Page>(new Page(index, context_, page));
}

const std::filesystem::path& Document::getPath() const {
    return path_;
}

Result<Document> Document::open(Context& context, const std::filesystem::path& path) {
    auto ctx = context.native();
    fz_document* doc = nullptr;

    PDFT_FZ_VERIFY(doc = fz_open_document(ctx, path.c_str()));
    PDFT_VERIFY(doc);
    return Document(context, path, doc);
}

}  // namespace pdft::media::mupdf
