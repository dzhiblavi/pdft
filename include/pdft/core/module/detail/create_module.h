#pragma once

#include "pdft/core/module/detail/constructor.h"
#include "pdft/core/module/detail/injector.h"

#include "pdft/core/result/coro/result.h"  // IWYU pragma: keep

namespace pdft::module::detail {

template <typename T, typename Type>
using Injector = std::conditional_t<Type::value, RefArgumentInjector<T>, ValueArgumentInjector<T>>;

template <IsModule T>
Result<Arc<T>> createModule(LoadContext& ctx, const ModuleConfig& config, const std::string& name) {
    using Parameters = typename detail::Constructor<T>::type;

    try {
        auto arguments = std::apply(
            [&]<typename... Types>(Types...) {
                InjectCtx inject{
                    .load = ctx,
                    .self = name,
                    .config = config,
                    .arg_index = 0,
                };

                return std::make_tuple(Injector<T, Types>(inject)...);
            },
            Parameters{});

        std::apply(
            [i = size_t(0)](auto&... args) mutable { ((args.ctx.arg_index = i++), ...); },
            arguments);

        co_return std::apply(
            [&, name](auto... args) { return std::make_shared<T>(std::move(args)...); },
            std::move(arguments));
    } catch (std::string error) {
        co_return result::err(error);
    }
}

}  // namespace pdft::module::detail
