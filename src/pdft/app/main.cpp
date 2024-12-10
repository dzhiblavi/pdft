#include "pdft/app/cli_args.h"
#include "pdft/app/config.h"

#include "pdft/core/log/load.h"
#include "pdft/core/module/factory_storage.h"
#include "pdft/core/module/loader.h"

#include "pdft/utils/get_home_dir.h"

#include <asio/co_spawn.hpp>
#include <asio/detached.hpp>
#include <asio/io_context.hpp>

#include <rfl/Processors.hpp>
#include <rfl/json/load.hpp>

#include <iostream>

namespace pdft::app {

void main(Config config, std::span<const char*> argv) {
    log::loadConfig(config.logging);

    asio::io_context context(1);

    module::LoadContext ctx{
        .argv = argv,
        .io = context,
        .modules = config.modules,
    };

    module::ModuleLoader loader(ctx);

    for (const auto& [name, conf] : ctx.modules) {
        if (loader.isLoaded(name)) {
            continue;
        }

        auto mod = loader.getOrLoadModule(name);
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

int main(int argc, const char** argv) {
    auto args = std::span(argv, argc);

    auto cli_args = pdft::app::parseCliArgs(args);
    if (!cli_args.has_value()) {
        std::cerr << std::format("Error while parsing CLI arguments: {}", cli_args.error())
                  << std::endl;
        return 1;
    }

    auto config = rfl::json::load<pdft::app::Config, rfl::DefaultIfMissing>(
        cli_args->config_path.value_or(pdft::utils::expandHome("~/.config/pdft/config.json")));

    if (!config) {
        std::cerr << std::format("Error while parsing configuration: {}", config.error()->what())
                  << std::endl;
        return 1;
    }

    pdft::app::main(*config, args);
    return 0;
}
