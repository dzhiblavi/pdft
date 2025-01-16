#pragma once

#include "platform/core/result.h"
#include <format>

extern "C" {
#include <mupdf/fitz/context.h>
}

namespace pdft::media {

template <std::invocable T>
requires(!std::is_same_v<void, std::invoke_result_t<T>>)
platform::Result<std::invoke_result_t<T>> tryFz(fz_context* ctx, T&& func) {
    platform::Result<std::invoke_result_t<T>> result{};

    fz_try(ctx) {
        // It is illegal to return from here
        result = std::forward<T>(func)();
    }
    fz_catch(ctx) {
        result = result::makeError(std::format("mupdf (fz) call failed: {}", ctx->error.message));
    }

    return result;
}

template <std::invocable T>
requires(std::is_same_v<void, std::invoke_result_t<T>>)
platform::Status tryFz(fz_context* ctx, T&& func) {
    platform::Status result;

    fz_try(ctx) {
        // It is illegal to return from here
        std::forward<T>(func)();
        result = platform::Unit{};
    }
    fz_catch(ctx) {
        result = result::makeError(std::format("mupdf (fz) call failed: {}", ctx->error.message));
    }

    return result;
}

}  // namespace pdft::media
