#pragma once

#include "pdft/utils/cli/detail/get_cli_args.h"

namespace pdft::cli::detail {

template <typename C>
Result<C> parsed(CliArgsFor<C>& cli_args) {
    C result;
    ConsumersFor<C> consumers = makeConsumers(result);

    auto apply = [&]<size_t I>() {
        auto& cli_arg = std::get<I>(cli_args);
        auto& consumer = std::get<I>(consumers);
        return cli_arg->apply(&consumer);
    };

    auto do_apply =
        [&]<typename Self, size_t I, size_t... Is>(
            this const Self& self, std::index_sequence<I, Is...>) -> Result<Unit, std::string> {
        if (auto ok = apply.template operator()<I>(); !ok) {
            return std::unexpected(std::move(ok.error()));
        }

        if constexpr (sizeof...(Is) != 0) {
            return self.template operator()(std::index_sequence<Is...>());
        } else {
            return unit;
        }
    };

    return do_apply(std::make_index_sequence<std::tuple_size_v<std::decay_t<decltype(cli_args)>>>())
        .transform([&](Unit) { return result; });
}

}  // namespace pdft::cli::detail
