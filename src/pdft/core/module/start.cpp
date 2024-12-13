#include "pdft/core/module/start.h"
#include "pdft/core/log/load.h"
#include "pdft/core/module/load/loader.h"
#include "pdft/core/result/combine/map.h"
#include "pdft/core/result/coro/result.h"
#include "pdft/core/result/syntax/pipe.h"

#include "pdft/app/config.h"

#include "pdft/utils/cli/cli.h"
#include "pdft/utils/cli/parse.h"
#include "pdft/utils/cli/parsed.h"
#include "pdft/utils/cli/register.h"
#include "pdft/utils/get_home_dir.h"

#include <rfl/Processors.hpp>
#include <rfl/json/load.hpp>

#include <asio/io_context.hpp>

namespace pdft::module {

namespace {

Status main(app::Config config) {
    log::loadConfig(config.logging);
    asio::io_context context(1);

    module::LoadContext ctx{
        .io = context,
        .modules = config.modules,
    };

    co_await module::loadAllModules(ctx);

    try {
        context.run();
        co_return unit;
    } catch (...) {
        co_return "an unexpected exception in main loop";
    }
}

struct CliArgs {
    cli::opt<std::optional<std::string>, 'c', "Path to the configuration file"> config_path;
};

}  // namespace

Result<int> start(
    const std::string& description, const std::string& version, std::span<const char*> argv) {
    co_await cli::parse(argv, description, version);
    auto cli_args = cli::parsed<CliArgs>();
    if (!cli_args) {
        co_return std::format("Failed to parse CLI arguments: {}", cli_args.error());
    }

    auto config = rfl::json::load<app::Config, rfl::DefaultIfMissing>(
        cli_args->config_path->value_or(utils::expandHome("~/.config/pdft/config.json")));
    if (!config) {
        co_return std::format("Error while parsing configuration: {}", config.error()->what());
    }

    co_return main(*config) | result::map([] { return 0; });
}

}  // namespace pdft::module

PDFT_REGISTER_CLI_ARGS(pdft::module::CliArgs);
