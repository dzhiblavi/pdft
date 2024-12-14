#pragma once

#include "pdft/core/module/detail/config_type.h"
#include "pdft/core/module/detail/injected.h"
#include "pdft/core/module/load.h"
#include "pdft/core/module/module.h"

#include "pdft/core/result/combine/map_err.h"
#include "pdft/core/result/coro/result.h"  // IWYU pragma: keep
#include "pdft/core/result/syntax/pipe.h"  // IWYU pragma: keep

#include "pdft/utils/cli/parsed.h"

#include <asio/io_context.hpp>
#include <rfl/from_generic.hpp>

namespace pdft::module::detail {

template <IsModule ModuleType, std::same_as<LoadContext&> ArgType>
ArgType injectArgument(LoadContext& ctx, const std::string) {
    return ctx;
}

template <IsModule ModuleType, std::same_as<asio::io_context&> ArgType>
ArgType injectArgument(LoadContext& ctx, const std::string&) {
    return ctx.getIoContext();
}

template <IsModule ModuleType, std::same_as<asio::any_io_executor> ArgType>
ArgType injectArgument(LoadContext& ctx, const std::string&) {
    return ctx.getIoContext().get_executor();
}

template <IsModule ModuleType, std::derived_from<InjectedTag> ArgType>
Result<ArgType> injectArgument(LoadContext& ctx, const std::string& name) {
    using T = typename ArgType::InjectedType;

    return ctx.template getModule<T>(ArgType::Name.c_str())  //
           | result::mapErr([&](auto err) {
                 return std::format(
                     "while loading module '{}' of type {} required by '{}':\n{}",
                     ArgType::Name.c_str(),
                     rfl::type_name_t<T>().name(),
                     name,
                     err);
             });
}

template <detail::HasCliArgs ModuleType, std::same_as<typename ModuleType::CliArgs> ArgType>
Result<ArgType> injectArgument(LoadContext&, const std::string& module) {
    auto result = cli::parsed<ArgType>();
    if (result.has_value()) {
        return *result;
    }

    return result::err(
        "while parsing CLI arguments of type '{}' required by '{}':\n{}",
        rfl::type_name_t<ArgType>().name(),
        module,
        result.error());
}

template <detail::HasConfig ModuleType, std::same_as<typename ModuleType::Config> ArgType>
Result<ArgType> injectArgument(LoadContext& ctx, const std::string& module) {
    auto maybe_config = ctx.getConfig(module);
    if (!maybe_config.has_value()) {
        return result::err(
            "while retrieving config of type '{}' required by '{}':\n{}",
            rfl::type_name_t<ArgType>().name(),
            module,
            "no configuration specified");
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
        "while parsing config of type '{}' required by '{}':\n{}",
        rfl::type_name_t<ArgType>().name(),
        module,
        result.error()->what());
}

}  // namespace pdft::module::detail
