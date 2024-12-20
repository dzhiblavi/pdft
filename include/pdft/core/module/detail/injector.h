#pragma once

#include "pdft/core/module/detail/inject.h"
#include "pdft/core/module/module.h"

#include "pdft/core/result/combine/map_err.h"
#include "pdft/core/result/syntax/pipe.h"  // IWYU pragma: keep

namespace pdft::module::detail {

template <typename T>
T&& unwrap(const InjectCtx&, T&& value) {
    return std::forward<T>(value);
}

template <typename T>
T unwrap(const InjectCtx& ctx, Result<T> value) {
    using result::mapErr;

    value = std::move(value) | mapErr([&](auto err) {
                return std::format(
                    "while loading argument of type {} (index {}) for module '{}':\n{}",
                    rfl::type_name_t<T>().name(),
                    ctx.arg_index,
                    ctx.self,
                    err);
            });

    if (!value.has_value()) {
        throw std::move(value.error());
    }

    return std::move(*value);
}

template <IsModule ModuleType>
struct ValueArgumentInjector {
    explicit ValueArgumentInjector(InjectCtx ctx) : ctx(ctx) {}

    template <typename T>
    requires(!std::is_same_v<std::decay_t<T>, ModuleType>)
    operator T() const {
        return unwrap(ctx, injectArgument<ModuleType, T>(ctx));
    }

    InjectCtx ctx;
};

template <IsModule ModuleType>
struct RefArgumentInjector {
    explicit RefArgumentInjector(InjectCtx ctx) : ctx(ctx) {}

    template <typename T>
    requires(!std::is_same_v<std::decay_t<T>, ModuleType>)
    operator T() const {
        return unwrap(ctx, injectArgument<ModuleType, T>(ctx));
    }

    template <typename T>
    requires(!std::is_same_v<std::decay_t<T>, ModuleType>)
    operator T&() const {
        return unwrap(ctx, injectArgument<ModuleType, T&>(ctx));
    }

    template <typename T>
    requires(!std::is_same_v<std::decay_t<T>, ModuleType>)
    operator T&&() const {
        return unwrap(ctx, injectArgument<ModuleType, T&&>(ctx));
    }

    template <typename T>
    requires(!std::is_same_v<std::decay_t<T>, ModuleType>)
    operator const T&() const {
        return unwrap(ctx, injectArgument<ModuleType, const T&>(ctx));
    }

    InjectCtx ctx;
};

}  // namespace pdft::module::detail
