#include "pdft/media/mupdf/context.h"
#include "pdft/core/assert.h"

extern "C" {
#include <mupdf/fitz/archive.h>
#include <mupdf/fitz/context.h>
#include <mupdf/fitz/document.h>
}

#include <utility>

namespace pdft::media::mupdf {

Context::Builder& Context::Builder::setStorageSize(int size) {
    size_ = size;
    return *this;
}

Context Context::Builder::build() {
    auto* context = fz_new_context(NULL, NULL, size_);
    PDFT_VERIFY(context, "failed to create new fz_context");
    return Context{context};
}

Context::Context(Context&& rhs) : context_(std::exchange(rhs.context_, nullptr)) {}

Context::~Context() noexcept {
    fz_drop_context(context_);
}

fz_context* Context::native() const {
    return context_;
}

void Context::registerDocumentHandlers() const {
    auto ctx = native();

    fz_try(ctx) fz_register_document_handlers(ctx);
    fz_catch(ctx) {
        core::fail("fz_register_document_handlers");
    }
}

Context& thisThreadContext() {
    thread_local Context context = Context::Builder().build();
    return context;
}

}  // namespace pdft::media::mupdf
