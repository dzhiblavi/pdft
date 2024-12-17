#pragma once

#include "pdft/core/cli/detail/cli.h"

namespace pdft::cli {

template <
    typename T,
    char Short = '\0',
    utils::FixedString Description = "",
    utils::FixedString Long = "">
using opt = detail::CliOption<T, detail::OptionDescription<Short, Description, Long>>;

}  // namespace pdft::cli
