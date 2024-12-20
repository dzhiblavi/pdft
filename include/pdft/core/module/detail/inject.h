#pragma once

#include "pdft/core/module/detail/config_type.h"
#include "pdft/core/module/load.h"
#include "pdft/core/module/module.h"

#include "pdft/core/cli/parsed.h"

#include <asio/io_context.hpp>
#include <rfl/from_generic.hpp>

namespace pdft::module::detail {

struct InjectCtx {
    LoadContext& load;
    const std::string& self;
    const ModuleConfig& config;
    size_t arg_index;
};

template <IsModule ModuleType, std::same_as<LoadContext&> ArgType>
ArgType injectArgument(const InjectCtx& ctx) {
    return ctx;
}

template <IsModule ModuleType, std::same_as<asio::io_context&> ArgType>
ArgType injectArgument(const InjectCtx& ctx) {
    return ctx.load.getIoContext();
}

template <IsModule ModuleType, std::same_as<asio::any_io_executor> ArgType>
ArgType injectArgument(const InjectCtx& ctx) {
    return ctx.load.getIoContext().get_executor();
}

template <detail::HasCliArgs ModuleType, std::same_as<typename ModuleType::CliArgs> ArgType>
Result<ArgType> injectArgument(const InjectCtx&) {
    return cli::parsed<ArgType>();
}

template <detail::HasConfig ModuleType, std::same_as<typename ModuleType::Config> ArgType>
Result<ArgType> injectArgument(const InjectCtx& ctx) {
    auto&& config = ctx.config.config;
    if (config.is_null()) {
        return ArgType{};
    }

    auto result = rfl::from_generic<ArgType, rfl::DefaultIfMissing>(config);
    if (result) {
        return *result;
    }

    return result::err(result.error()->what());
}

template <IsModule ModuleType, utils::InstanceOfTemplate<std::shared_ptr> ArgType>
Result<ArgType> injectArgument(const InjectCtx& ctx) {
    using T = typename ArgType::element_type;
    auto type_name = rfl::type_name_t<std::decay_t<T>>().name();

    auto&& deps = ctx.config.deps;

    if (ctx.arg_index >= deps.size()) {
        return result::err(
            "dependency index {} is out of range [0..{})", ctx.arg_index, deps.size());
    }

    const auto& dep = deps[ctx.arg_index];

    if (auto* dep_name = std::get_if<std::string>(&dep)) {
        return ctx.load.getModule<T>(*dep_name);
    } else {
        const auto anon_module_name =
            std::format("anon(type={},owner={},idx={})", type_name, ctx.self, ctx.arg_index);
        const ModuleConfig& dep_conf = std::get<ModuleConfig>(dep);

        return ctx.load.loadModule<T>(anon_module_name, dep_conf);
    }
}

}  // namespace pdft::module::detail
