#pragma once

#include "pdft/core/module/config.h"
#include "pdft/core/module/detail/config_type.h"
#include "pdft/core/module/detail/constructor.h"
#include "pdft/core/module/inject.h"
#include "pdft/core/module/module.h"

#include <rfl/Generic.hpp>
#include <rfl/from_generic.hpp>

#include <asio/io_context.hpp>

#include <span>

namespace pdft::module {

using LoadedModules = std::unordered_map<std::string, Arc<Module>>;

struct LoadContext {
    std::span<const char*> argv;
    asio::io_context& io;
    ModulesConfig modules;
    LoadedModules loaded = {};
};

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
            return ArgType{getOrLoadModule(ArgType::Name.c_str())};
        } else {
            []<bool flag = false>() {
                static_assert(flag, "I don't know how to get you this constructor argument");
            }();
        }
    }

    template <IsModule T>
    Arc<T> makeModule(std::string name);

    Arc<Module> getOrLoadModule(const std::string& name);

    bool isLoaded(const std::string& name);

 private:
    const ModuleConfig& getConfig(const std::string& name);
    Arc<Module> addNewModule(const std::string& name, Arc<Module> module);

    LoadContext& context_;
};

template <IsModule ModuleType>
struct ValueArgumentInjector {
    using ConfigType = detail::MaybeConfigType<ModuleType>;

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
    using ConfigType = detail::MaybeConfigType<ModuleType>;

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

template <IsModule T>
Arc<T> ModuleLoader::makeModule(std::string name) {
    using Parameters = typename detail::Constructor<T>::type;

    Arc<T> module = std::apply(
        [this, name]<typename... Types>(Types...) {
            return std::make_shared<T>(
                std::conditional_t<Types::value, RefArgumentInjector<T>, ValueArgumentInjector<T>>(
                    this, name)...);
        },
        Parameters{});

    const auto& config = getConfig(name);

    module->setLogger(PDFT_VERIFY(
        log::logger(config.logger),
        "missing logger {} required by module {}",
        config.logger,
        name));

    module->setName(std::move(name));
    return module;
}

}  // namespace pdft::module
