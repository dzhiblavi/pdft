#pragma once

#include "pdft/core/module/config.h"
#include "pdft/core/module/detail/constructor.h"
#include "pdft/core/module/inject.h"
#include "pdft/core/module/module.h"
#include "pdft/core/result/type/unit.h"

#include <asio/io_context.hpp>
#include <rfl/Generic.hpp>
#include <rfl/from_generic.hpp>
#include <yaml-cpp/node/node.h>

#include <span>

namespace pdft {

using LoadedModules = std::unordered_map<std::string, Arc<Module>>;

struct LoadContext {
    std::span<const char*> argv;
    asio::io_context& io;
    ModulesConfig modules;
    LoadedModules loaded = {};
};

template <typename T, typename = void>
struct MaybeConfigTypeImpl {
    using type = Unit;
};

template <typename T>
struct MaybeConfigTypeImpl<T, std::void_t<typename T::Config>> {
    using type = typename T::Config;
};

template <typename T>
using MaybeConfigType = typename MaybeConfigTypeImpl<T>::type;

class ModuleLoader {
 public:
    explicit ModuleLoader(LoadContext& context) : context_(context) {}

    template <typename ArgType, typename ConfigType>
    ArgType injectArgument(const std::string for_module) {
        if constexpr (std::is_same_v<ArgType, asio::io_context&>) {
            return context_.io;
        } else if constexpr (std::is_same_v<ArgType, asio::any_io_executor>) {
            return context_.io.get_executor();
        } else if constexpr (std::is_same_v<ArgType, std::span<const char*>>) {
            return context_.argv;
        } else if constexpr (std::is_same_v<ArgType, ConfigType>) {
            auto&& config = getConfig(for_module).config;

            if (config.is_null()) {
                return ConfigType{};
            }

            auto result = rfl::from_generic<ConfigType, rfl::DefaultIfMissing>(config);
            return *PDFT_VERIFY(
                result, "failed to parse config for {}: {}", for_module, result.error()->what());
        } else if constexpr (std::convertible_to<ArgType&, DependsOnModuleTag&>) {
            return ArgType{loadModule(ArgType::Name.c_str())};
        } else {
            []<bool flag = false>() {
                static_assert(flag, "I don't know how to get you this constructor argument");
            }();
        }
    }

    template <typename T>
    Arc<Module> loadModule(std::string name);

    Arc<Module> loadModule(const std::string& name);

 private:
    const ModuleConfig& getConfig(const std::string& name);
    Arc<Module> addNewModule(const std::string& name, ModulePtr module);

    LoadContext& context_;
};

template <IsModule ModuleType>
struct ValueArgumentInjector {
    using ConfigType = MaybeConfigType<ModuleType>;

    ValueArgumentInjector(ModuleLoader* ctx, const std::string& name) : ctx(ctx), name(name) {}

    template <typename T>
    requires(!std::is_same_v<std::decay_t<T>, ModuleType>)
    operator T() const {
        return ctx->template injectArgument<T, ConfigType>(name);
    }

    ModuleLoader* ctx;
    std::string name;
};

template <IsModule ModuleType>
struct RefArgumentInjector {
    using ConfigType = MaybeConfigType<ModuleType>;

    RefArgumentInjector(ModuleLoader* ctx, const std::string& name) : ctx(ctx), name(name) {}

    template <typename T>
    requires(!std::is_same_v<std::decay_t<T>, ModuleType>)
    operator T() const {
        return ctx->template injectArgument<T, ConfigType>(name);
    }
    template <typename T>
    requires(!std::is_same_v<std::decay_t<T>, ModuleType>)
    operator T&() const {
        return ctx->template injectArgument<T&, ConfigType>(name);
    }
    template <typename T>
    requires(!std::is_same_v<std::decay_t<T>, ModuleType>)
    operator T&&() const {
        return ctx->template injectArgument<T&&, ConfigType>(name);
    }
    template <typename T>
    requires(!std::is_same_v<std::decay_t<T>, ModuleType>)
    operator const T&() const {
        return ctx->template injectArgument<const T&, ConfigType>(name);
    }

    ModuleLoader* ctx;
    std::string name;
};

template <typename T>
Arc<Module> ModuleLoader::loadModule(std::string name) {
    using Parameters = typename detail::Constructor<T>::type;

    return addNewModule(
        name,
        std::apply(
            [this, name]<typename... Types>(Types...) {
                return std::make_shared<T>(std::conditional_t<
                                           Types::value,
                                           RefArgumentInjector<T>,
                                           ValueArgumentInjector<T>>(this, name)...);
            },
            Parameters{}));
}

}  // namespace pdft
