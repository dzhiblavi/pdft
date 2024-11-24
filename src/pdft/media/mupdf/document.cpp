#include "pdft/media/mupdf/document.h"
#include "pdft/core/assert.h"
#include "pdft/media/mupdf/page.h"

extern "C" {
#include <mupdf/fitz/archive.h>
#include <mupdf/fitz/document.h>
}

#include <format>

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

    fz_try(ctx) page_count = fz_count_pages(ctx, document_);
    fz_catch(ctx) {
        core::fail("fz_count_pages");
    }

    return page_count;
}

std::shared_ptr<media::Page> Document::getPage(int index) const {
    PDFT_VERIFY(index >= 0 && index < static_cast<int>(getPageCount()));
    auto ctx = context_.native();
    fz_page* page = nullptr;

    fz_try(ctx) page = fz_load_page(ctx, document_, index);
    fz_catch(ctx) {
        core::fail("fz_load_page");
    }

    if (!page) {
        core::fail("fz_load_page is null");
    }

    return std::shared_ptr<media::Page>(new Page(index, context_, page));
}

const std::filesystem::path& Document::getPath() const {
    return path_;
}

Document Document::open(Context& context, const std::filesystem::path& path) {
    auto ctx = context.native();
    fz_document* doc = nullptr;

    fz_try(ctx) doc = fz_open_document(ctx, path.c_str());
    fz_catch(ctx) {
        throw std::runtime_error(std::format("failed to open a document by path {}", path.c_str()));
    }

    if (!doc) {
        core::fail("fz_open_document is null");
    }

    return {context, path, doc};
}

}  // namespace pdft::media::mupdf
