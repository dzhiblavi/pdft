#pragma once

#include "pdft/core/module/load/detail/inject.h"

namespace pdft::module::detail {

template <IsModule ModuleType>
struct ValueArgumentInjector {
    ValueArgumentInjector(LoadContext& ctx, const std::string& name) : ctx(ctx), name(name) {}

    template <typename T>
    requires(!std::is_same_v<std::decay_t<T>, ModuleType>)
    operator T() const {
        return injectArgument<ModuleType, T>(ctx, name);
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
        return injectArgument<ModuleType, T>(ctx, name);
    }

    template <typename T>
    requires(!std::is_same_v<std::decay_t<T>, ModuleType>)
    operator T&() const {
        return injectArgument<ModuleType, T&>(ctx, name);
    }

    template <typename T>
    requires(!std::is_same_v<std::decay_t<T>, ModuleType>)
    operator T&&() const {
        return injectArgument<ModuleType, T&&>(ctx, name);
    }

    template <typename T>
    requires(!std::is_same_v<std::decay_t<T>, ModuleType>)
    operator const T&() const {
        return injectArgument<ModuleType, const T&>(ctx, name);
    }

    LoadContext& ctx;
    std::string name;
};

}  // namespace pdft::module::detail
