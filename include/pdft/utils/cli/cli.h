#pragma once

#include "pdft/utils/cli/detail/cli.h"

namespace pdft::cli {

template <typename T, char Short, utils::FixedString Description = "", utils::FixedString Long = "">
using opt = detail::CliOption<T, detail::OptionDescription<Short, Description, Long>>;

}  // namespace pdft::cli
