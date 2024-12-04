#include "pdft/media/mupdf/context.h"
#include "pdft/core/assert.h"

#include "fz_assert.h"

extern "C" {
#include <mupdf/fitz/archive.h>
#include <mupdf/fitz/context.h>
#include <mupdf/fitz/document.h>
}

#include <utility>

namespace pdft::media {

MupdfContext::Builder& MupdfContext::Builder::setStorageSize(int size) {
    size_ = size;
    return *this;
}

MupdfContext MupdfContext::Builder::build() {
    auto* ctx = fz_new_context(NULL, NULL, size_);
    PDFT_VERIFY(ctx, "failed to create new fz_context");

    PDFT_FZ_VERIFY(fz_enable_icc(ctx));
    PDFT_FZ_VERIFY(fz_set_text_aa_level(ctx, 8));
    PDFT_FZ_VERIFY(fz_set_graphics_aa_level(ctx, 8));
    return MupdfContext{ctx};
}

MupdfContext::MupdfContext(MupdfContext&& rhs) : context_(std::exchange(rhs.context_, nullptr)) {}

MupdfContext::~MupdfContext() noexcept {
    fz_drop_context(context_);
}

fz_context* MupdfContext::native() const {
    return context_;
}

void MupdfContext::registerDocumentHandlers() const {
    auto ctx = native();
    PDFT_FZ_VERIFY(fz_register_document_handlers(ctx));
}

MupdfContext& MupdfContext::thisThreadInstance() {
    thread_local MupdfContext context = MupdfContext::Builder().build();
    return context;
}

}  // namespace pdft::media
