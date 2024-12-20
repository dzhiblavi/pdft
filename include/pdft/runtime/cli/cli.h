#pragma once

#include "pdft/runtime/cli/detail/cli.h"

namespace pdft::rt {

template <
    typename T,
    char Short = '\0',
    utils::FixedString Description = "",
    utils::FixedString Long = "">
using cli_opt = detail::CliOption<T, detail::OptionDescription<Short, Description, Long>>;

}  // namespace pdft::rt
