#pragma once

#include "pdft/utils/cli/detail/get_cli_args.h"

namespace pdft::cli::detail {

template <typename C>
static Result<C> parsed() {
    C result;

    // tuple<unique_ptr<CliArg<T, D>>...>&
    auto& cli_args = detail::getCliArgs<C>();

    // tuple<IConsumer<T> auto>...>
    auto consumers = detail::makeConsumers(result);

    auto apply = [&]<size_t I>() {
        auto& cli_arg = std::get<I>(cli_args);
        auto& consumer = std::get<I>(consumers);
        return cli_arg->apply(&consumer);
    };

    auto do_apply =
        [&]<typename Self, size_t I, size_t... Is>(
            this Self&& self, std::index_sequence<I, Is...>) -> Result<Unit, std::string> {
        if (auto ok = apply.template operator()<I>(); !ok) {
            return std::unexpected(std::move(ok.error()));
        }

        if constexpr (sizeof...(Is) != 0) {
            return self.template operator()<Is...>();
        } else {
            return unit;
        }
    };

    return do_apply(std::make_index_sequence<std::tuple_size_v<std::decay_t<decltype(cli_args)>>>())
        .transform([&](Unit) { return result; });
}

}  // namespace pdft::cli::detail
