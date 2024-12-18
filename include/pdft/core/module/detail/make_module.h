#pragma once

#include "pdft/core/module/detail/constructor.h"
#include "pdft/core/module/detail/injector.h"

#include "pdft/core/result/combine/lift.h"
#include "pdft/core/result/coro/result.h"  // IWYU pragma: keep

namespace pdft::module::detail {

template <IsModule T>
Result<Arc<T>> createModule(LoadContext& ctx, std::string name) {
    using Parameters = typename detail::Constructor<T>::type;

    try {
        co_return std::apply(
            [&, name]<typename... Types>(Types...) {
                return std::make_shared<T>(std::conditional_t<
                                           Types::value,
                                           RefArgumentInjector<T>,
                                           ValueArgumentInjector<T>>(ctx, name)...);
            },
            Parameters{});
    } catch (std::string error) {
        co_return result::err(error);
    }
}

template <IsModule T>
Result<Arc<T>> loadModule(LoadContext& ctx, std::string name) {
    auto* config = co_await (
        ctx.getConfig(name) |
        result::lift(
            "module '{}' of type {} is not configured", name, rfl::type_name_t<T>().name()));

    Arc<T> module = co_await createModule<T>(ctx, name);

    log::LoggerPtr logger;

    if (config->logger != "global") {
        logger = log::logger(config->logger);
    } else {
        logger = log::global();
    }

    if (!logger) {
        co_return result::err(
            "missing logger {} required by module '{}' of type {}",
            config->logger,
            name,
            rfl::type_name_t<T>().name());
    }

    module->setLogger(logger);
    module->setName(std::move(name));
    co_return module;
}

}  // namespace pdft::module::detail
