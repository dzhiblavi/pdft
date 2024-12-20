#pragma once

// IWYU pragma: private, include "register.h"

#include "pdft/runtime/cli/detail/cli_arg.h"
#include "pdft/runtime/cli/detail/get_cli_args.h"
#include "pdft/runtime/cli/storage.h"

namespace pdft::rt::detail {

template <typename C>
void addOptions(OptionsStorage& storage, CliArgsFor<C>& cli_args) {
    auto add = [&]<typename T, typename D>(CliArg<D, T>& arg) {
        storage.addOption(D::Short, &arg);
    };

    std::apply([&](auto&... args) { (add(*args), ...); }, cli_args);
}

template <typename C>
struct CliRegisterer {
    CliRegisterer() {
        addOptions<C>(OptionsStorage::globalInstance(), getGlobalCliArgs<C>());
    }
};

}  // namespace pdft::rt::detail
