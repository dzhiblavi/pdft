#include "pdft/app/config.h"
#include "pdft/app/version.h"

#include "pdft/core/log/load.h"
#include "pdft/core/module/factory_storage.h"
#include "pdft/core/module/load/loader.h"
#include "pdft/core/result/combine/unwrap.h"
#include "pdft/core/result/syntax/pipe.h"

#include "pdft/utils/cli/parsed.h"
#include "pdft/utils/cli/register.h"
#include "pdft/utils/get_home_dir.h"

#include <asio/co_spawn.hpp>
#include <asio/detached.hpp>
#include <asio/io_context.hpp>

#include <rfl/Processors.hpp>
#include <rfl/json/load.hpp>

#include <iostream>

namespace pdft::app {

void main(Config config) {
    log::loadConfig(config.logging);

    asio::io_context context(1);

    module::LoadContext ctx{
        .io = context,
        .modules = config.modules,
    };

    for (const auto& [name, conf] : ctx.modules) {
        if (module::isLoaded(ctx, name)) {
            continue;
        }

        auto mod = module::getOrLoadModule(ctx, name);
        auto traits = module::FactoryStorage::globalInstance().getTraits(conf.factory);
        traits->start(mod.get());
    }

    try {
        context.run();
    } catch (...) {
        panic("an unexpected exception");
    }
}

}  // namespace pdft::app

using namespace pdft;

struct CliArgs {
    cli::cli<std::optional<std::string>, 'c', "Path to the configuration file"> config_path;
};

int main(int argc, const char** argv) {
    TCLAP::CmdLine cmd("PDFT: A command line PDF reader", ' ', app::getFullVersionString());
    cli::OptionsStorage::globalInstance().attach(cmd);

    try {
        cmd.parse(argc, argv);
    } catch (const TCLAP::ArgException& e) {
        std::cerr << std::format("Error for argument '{}': {}", e.argId(), e.error()) << std::endl;
        return 1;
    }

    auto cli_args = cli::parsed<CliArgs>() | result::unwrap;

    auto config = rfl::json::load<app::Config, rfl::DefaultIfMissing>(
        cli_args.config_path->value_or(utils::expandHome("~/.config/pdft/config.json")));

    if (!config) {
        std::cerr << std::format("Error while parsing configuration: {}", config.error()->what())
                  << std::endl;
        return 1;
    }

    app::main(*config);
    return 0;
}

PDFT_REGISTER_CLI_ARGS(CliArgs);
