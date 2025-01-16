#include "pdft/media/mupdf/context.h"
#include "platform/core/assert.h"

#include "fz_assert.h"
#include "platform/module/register.h"

extern "C" {
#include <mupdf/fitz/archive.h>
#include <mupdf/fitz/context.h>
#include <mupdf/fitz/document.h>
}

namespace pdft::media {

namespace {

void warningCallback(void* raw, const char* message) {
    auto* logger = static_cast<spdlog::logger*>(raw);
    logger->warn(message);
}

void errorCallback(void* raw, const char* message) {
    auto* logger = static_cast<spdlog::logger*>(raw);
    logger->error(message);
}

void setupMupdfLogging(fz_context* ctx, platform::log::LoggerPtr logger) {
    fz_set_warning_callback(ctx, warningCallback, logger.get());
    fz_set_error_callback(ctx, errorCallback, logger.get());
}

void registerDocumentHandlers(fz_context* ctx) {
    platform::verify(tryFz(ctx, [&] { fz_register_document_handlers(ctx); }), "");
}

void fzLockUser(void* user, int lock) {
    auto span = static_cast<std::mutex*>(user);
    span[lock].lock();  // NOLINT
}

void fzUnlockUser(void* user, int lock) {
    auto span = static_cast<std::mutex*>(user);
    span[lock].unlock();  // NOLINT
}

platform::Arc<MupdfContext> global_context = {};  // NOLINT

}  // namespace

void MupdfContext::drop(fz_context* ctx) noexcept {
    fz_drop_context(ctx);
}

MupdfContextOwner::MupdfContextOwner(Config config) : config_(config) {
    static_assert(FZ_LOCK_MAX == 3, "fz locks count mismatch");

    fz_locks_context locks_ctx{
        .user = locks_.data(),
        .lock = fzLockUser,
        .unlock = fzUnlockUser,
    };

    context_.reset(fz_new_context(NULL, &locks_ctx, static_cast<int>(config.size)));
    platform::verify(native(), "failed to create new fz_context");
}

void MupdfContextOwner::start() {
    auto* ctx = native();

    if (config_.enable_icc) {
        platform::verify(tryFz(ctx, [&] { fz_enable_icc(ctx); }), "failed to enable icc");
    }

    if (config_.text_aa_level != Config::UseDefault) {
        platform::verify(
            tryFz(ctx, [&] { fz_set_text_aa_level(ctx, config_.text_aa_level); }),
            "failed to set text aa level");
    }

    if (config_.graphics_aa_level != Config::UseDefault) {
        platform::verify(
            tryFz(ctx, [&] { fz_set_graphics_aa_level(ctx, config_.graphics_aa_level); }),
            "failed to set graphics aa level");
    }

    if (config_.setup_logging) {
        setupMupdfLogging(ctx, getLogger());
    }

    registerDocumentHandlers(ctx);

    if (config_.register_as_global) {
        if (auto ptr = shared_from_this()) {
            setGlobalMupdfContext(ptr);
        } else {
            platform::panic("cannot set MupdfContextOwner as global unless it is shared");
        }
    }
}

fz_context* MupdfContextOwner::native() const {
    return context_.get();
}

ClonedMupdfContext::ClonedMupdfContext(fz_context* context, platform::Arc<MupdfContext> parent)
    : parent_(std::move(parent)) {
    context_.reset(platform::verify(context));
}

fz_context* ClonedMupdfContext::native() const {
    return context_.get();
}

platform::Arc<MupdfContext> ClonedMupdfContext::clone(platform::Arc<MupdfContext> from) {
    fz_context* ctx = platform::verify(fz_clone_context(from->native()), "failed to clone context");
    return std::make_shared<ClonedMupdfContext>(ctx, std::move(from));
}

void setGlobalMupdfContext(platform::Arc<MupdfContext> ctx) {
    if (global_context != nullptr) {
        platform::panic("global MuPdf context is already registered (@{})", (void*)ctx.get());
    }

    global_context = ctx;
}

MupdfContext& thisThreadMupdfContext() {
    thread_local platform::Arc<MupdfContext> local = [] {
        if (global_context == nullptr) {
            platform::panic("global MuPdf context is not registered");
        }

        return ClonedMupdfContext::clone(global_context);
    }();

    return *local;
}

}  // namespace pdft::media

PLATFORM_REGISTER_MODULE(pdft::media::MupdfContextOwner);
