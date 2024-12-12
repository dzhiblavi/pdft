#pragma once

#include "pdft/utils/cli/detail/cli_arg.h"
#include "pdft/utils/cli/detail/get_cli_args.h"
#include "pdft/utils/cli/storage.h"
#include "pdft/utils/anon_var_name.h"

namespace pdft::cli::detail {

template <typename C>
static void addOptions(OptionsStorage& storage) {
    auto add = [&]<typename T, typename D>(detail::CliArg<D, T>& arg) {
        storage.addOption(D::Short, &arg);
    };

    std::apply([&](auto&... args) { (add(*args), ...); }, getCliArgs<C>());
}

template <typename C>
struct CliRegisterer {
    CliRegisterer() {
        addOptions<C>(OptionsStorage::globalInstance());
    }
};

}  // namespace pdft::cli::detail

#define PDFT_REGISTER_CLI_ARGS(Cli) \
    [[maybe_unused]] const auto ANON_VAR(cli_registerer) = ::pdft::cli::detail::CliRegisterer<Cli>()
