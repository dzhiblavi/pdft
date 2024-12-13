#include "pdft/core/module/start.h"
#include "pdft/core/log/load.h"
#include "pdft/core/module/load/loader.h"
#include "pdft/core/result/combine/map.h"
#include "pdft/core/result/coro/result.h"
#include "pdft/core/result/make/err.h"
#include "pdft/core/result/make/ok.h"
#include "pdft/core/result/syntax/pipe.h"

#include "pdft/app/config.h"

#include "pdft/utils/cli/parsed.h"
#include "pdft/utils/cli/register.h"
#include "pdft/utils/cli/storage.h"
#include "pdft/utils/get_home_dir.h"

#include <asio/io_context.hpp>
#include <rfl/Processors.hpp>
#include <rfl/json/load.hpp>
#include <tclap/CmdLine.h>

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
    cli::cli<std::optional<std::string>, 'c', "Path to the configuration file"> config_path;
};

}  // namespace

Result<int> start(
    const std::string& description, const std::string& version, std::span<const char*> argv) {
    TCLAP::CmdLine cmd(description, ' ', version);
    cmd.setExceptionHandling(false);

    cli::OptionsStorage::globalInstance().attach(cmd);

    try {
        cmd.parse(argv.size(), argv.data());
    } catch (const TCLAP::ArgException& e) {
        return result::err(std::format("Error for argument '{}': {}", e.argId(), e.error()));
    } catch (const TCLAP::ExitException& e) {
        return result::ok(e.getExitStatus());
    }

    auto cli_args = cli::parsed<CliArgs>();

    if (!cli_args) {
        return result::err(std::format("Failed to parse CLI arguments: {}", cli_args.error()));
    }

    auto config = rfl::json::load<app::Config, rfl::DefaultIfMissing>(
        cli_args->config_path->value_or(utils::expandHome("~/.config/pdft/config.json")));

    if (!config) {
        return result::err(
            std::format("Error while parsing configuration: {}", config.error()->what()));
    }

    return main(*config) | result::map([] { return 0; });
}

}  // namespace pdft::module

PDFT_REGISTER_CLI_ARGS(pdft::module::CliArgs);
