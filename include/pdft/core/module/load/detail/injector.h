#pragma once

#include "pdft/core/module/load/detail/inject.h"
#include "pdft/core/result/combine/unwrap.h"
#include "pdft/core/result/syntax/pipe.h"  // IWYU pragma: keep

namespace pdft::module::detail {

template <typename T>
T&& unwrap(T&& value) {
    return std::forward<T>(value);
}

template <typename T>
T unwrap(Result<T> value) {
    return std::move(value) | result::unwrap;
}

template <IsModule ModuleType>
struct ValueArgumentInjector {
    ValueArgumentInjector(LoadContext& ctx, const std::string& name) : ctx(ctx), name(name) {}

    template <typename T>
    requires(!std::is_same_v<std::decay_t<T>, ModuleType>)
    operator T() const {
        return unwrap(injectArgument<ModuleType, T>(ctx, name));
    }

    LoadContext& ctx;
    std::string name;
};

template <IsModule ModuleType>
struct RefArgumentInjector {
    RefArgumentInjector(LoadContext& ctx, const std::string& name) : ctx(ctx), name(name) {}

    template <typename T>
    requires(!std::is_same_v<std::decay_t<T>, ModuleType>)
    operator T() const {
        return unwrap(injectArgument<ModuleType, T>(ctx, name));
    }

    template <typename T>
    requires(!std::is_same_v<std::decay_t<T>, ModuleType>)
    operator T&() const {
        return unwrap(injectArgument<ModuleType, T&>(ctx, name));
    }

    template <typename T>
    requires(!std::is_same_v<std::decay_t<T>, ModuleType>)
    operator T&&() const {
        return unwrap(injectArgument<ModuleType, T&&>(ctx, name));
    }

    template <typename T>
    requires(!std::is_same_v<std::decay_t<T>, ModuleType>)
    operator const T&() const {
        return unwrap(injectArgument<ModuleType, const T&>(ctx, name));
    }

    LoadContext& ctx;
    std::string name;
};

}  // namespace pdft::module::detail
