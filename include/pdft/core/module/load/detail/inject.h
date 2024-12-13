#pragma once

#include "pdft/core/module/load/loader.h"

#include "pdft/core/module/detail/config_type.h"
#include "pdft/core/module/detail/inject.h"

#include "pdft/core/result/coro/result.h"  // IWYU pragma: keep

#include "pdft/utils/cli/parsed.h"

#include <rfl/from_generic.hpp>

namespace pdft::module::detail {

template <IsModule ModuleType, std::same_as<asio::io_context&> ArgType>
ArgType injectArgument(LoadContext& ctx, const std::string&) {
    return ctx.io;
}

template <IsModule ModuleType, std::same_as<asio::any_io_executor> ArgType>
ArgType injectArgument(LoadContext& ctx, const std::string&) {
    return ctx.io.get_executor();
}

template <IsModule ModuleType, std::derived_from<InjectedTag> ArgType>
Result<ArgType> injectArgument(LoadContext& ctx, const std::string& name) {
    using T = typename ArgType::InjectedType;

    auto module = getOrLoadModule(ctx, ArgType::Name.c_str());
    if (!module.has_value()) {
        return result::err(
            "failed to construct injected module '{}' of type '{}': {} (required by '{}')",
            ArgType::Name.c_str(),
            rfl::type_name_t<T>().name(),
            module.error(),
            name);
    }

    if (auto casted = std::dynamic_pointer_cast<T>(std::move(*module))) {
        return ArgType{std::move(casted)};
    }

    return result::err(
        "injected module '{}' does not match the expected type '{}' (required by '{}')",
        ArgType::Name.c_str(),
        rfl::type_name_t<T>().name(),
        name);
}

template <detail::HasCliArgs ModuleType, std::same_as<typename ModuleType::CliArgs> ArgType>
Result<ArgType> injectArgument(LoadContext&, const std::string& module) {
    auto result = cli::parsed<ArgType>();

    if (result.has_value()) {
        return *result;
    }

    return result::err(
        "failed to parse CLI arguments of type '{}': {} (required by '{}')",
        rfl::type_name_t<ArgType>().name(),
        result.error(),
        module);
}

template <detail::HasConfig ModuleType, std::same_as<typename ModuleType::Config> ArgType>
Result<ArgType> injectArgument(LoadContext& ctx, const std::string& module) {
    auto maybe_config = getConfig(ctx, module);
    if (!maybe_config.has_value()) {
        return result::err(
            "failed to retrieve config of type '{}': {} (required by '{}')",
            rfl::type_name_t<ArgType>().name(),
            "no configuration specified",
            module);
    }
    auto&& config = (*maybe_config)->config;
    if (config.is_null()) {
        return ArgType{};
    }

    auto result = rfl::from_generic<ArgType, rfl::DefaultIfMissing>(config);
    if (result) {
        return *result;
    }

    return result::err(
        "failed to retrieve config of type '{}': {} (required by '{}')",
        rfl::type_name_t<ArgType>().name(),
        result.error()->what(),
        module);
}

}  // namespace pdft::module::detail
