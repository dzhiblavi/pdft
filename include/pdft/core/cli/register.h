#pragma once

#include "pdft/utils/anon_var_name.h"
#include "pdft/core/cli/detail/register.h"

#define PDFT_REGISTER_CLI_ARGS(Cli) \
    [[maybe_unused]] const auto ANON_VAR(cli_registerer) = ::pdft::cli::detail::CliRegisterer<Cli>()
