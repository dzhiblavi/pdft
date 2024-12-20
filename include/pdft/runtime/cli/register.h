#pragma once

#include "pdft/runtime/cli/detail/register.h"
#include "pdft/utils/anon_var_name.h"

#define PDFT_REGISTER_CLI_ARGS(Cli) \
    [[maybe_unused]] const auto ANON_VAR(cli_registerer) = ::pdft::rt::detail::CliRegisterer<Cli>()
