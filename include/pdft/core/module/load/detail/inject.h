#pragma once

#include "pdft/core/module/config.h"
#include "pdft/core/module/detail/config_type.h"
#include "pdft/core/module/inject.h"
#include "pdft/core/module/load/load_context.h"

#include "pdft/utils/cli/parsed.h"

#include <rfl/from_generic.hpp>

namespace pdft::module {

const ModuleConfig& getConfig(LoadContext& ctx, const std::string& name);

template <IsModule ModuleType, std::same_as<asio::io_context&> ArgType>
ArgType injectArgument(LoadContext& ctx, const std::string&) {
    return ctx.io;
}

template <IsModule ModuleType, std::same_as<asio::any_io_executor> ArgType>
ArgType injectArgument(LoadContext& ctx, const std::string&) {
    return ctx.io.get_executor();
}

template <IsModule ModuleType, std::derived_from<detail::DependsOnModuleTag> ArgType>
ArgType injectArgument(LoadContext& ctx, const std::string&) {
    return ArgType{getOrLoadModule(ctx, ArgType::Name.c_str())};
}

template <detail::HasCliArgs ModuleType, std::same_as<typename ModuleType::CliArgs> ArgType>
ArgType injectArgument(LoadContext&, const std::string& module) {
    using CliArgsType = typename ModuleType::CliArgs;

    auto result = cli::parsed<CliArgsType>();
    return *PDFT_VERIFY(result, "bad cli arguments for module {}: {}", module, result.error());
}

template <detail::HasConfig ModuleType, std::same_as<typename ModuleType::Config> ArgType>
ArgType injectArgument(LoadContext& ctx, const std::string& module) {
    using ConfigType = typename ModuleType::Config;

    auto&& config = getConfig(ctx, module).config;
    if (config.is_null()) {
        return ConfigType{};
    }

    auto result = rfl::from_generic<ConfigType, rfl::DefaultIfMissing>(config);
    return *PDFT_VERIFY(
        result, "failed to parse config for module {}: {}", module, result.error()->what());
}

}  // namespace pdft::module
