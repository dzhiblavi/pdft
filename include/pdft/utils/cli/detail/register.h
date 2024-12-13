#pragma once

// IWYU pragma: private, include "register.h"

#include "pdft/utils/cli/detail/cli_arg.h"
#include "pdft/utils/cli/detail/get_cli_args.h"
#include "pdft/utils/cli/detail/storage.h"

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
