#pragma once

#include "pdft/core/module/load/detail/constructor.h"
#include "pdft/core/module/load/detail/injector.h"
#include "pdft/core/result/combine/lift.h"
#include "pdft/core/result/coro/result.h"  // IWYU pragma: keep

namespace pdft::module::detail {

template <IsModule T>
Result<Arc<T>> makeModule(LoadContext& ctx, std::string name) {
    using Parameters = typename detail::Constructor<T>::type;

    auto* config = co_await (
        getConfig(ctx, name) |
        result::lift(
            "module '{}' of type {} is not configured", name, rfl::type_name_t<T>().name()));

    Arc<T> module = std::apply(
        [&, name]<typename... Types>(Types...) {
            return std::make_shared<T>(
                std::conditional_t<Types::value, RefArgumentInjector<T>, ValueArgumentInjector<T>>(
                    ctx, name)...);
        },
        Parameters{});

    module->setLogger(PDFT_VERIFY(
        log::logger(config->logger),
        "missing logger {} required by module {}",
        config->logger,
        name));
    module->setName(std::move(name));

    co_return module;
}

}  // namespace pdft::module::detail
