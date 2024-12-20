#include "pdft/runtime/module/start.h"
#include "pdft/runtime/config.h"
#include "pdft/runtime/module/load.h"

#include "pdft/runtime/cli/cli.h"
#include "pdft/runtime/cli/parse.h"
#include "pdft/runtime/cli/parsed.h"
#include "pdft/runtime/cli/register.h"

#include "pdft/core/log/load.h"
#include "pdft/core/result/coro/result.h"  // IWYU pragma: keep

#include "pdft/utils/get_home_dir.h"

#include <rfl/Processors.hpp>
#include <rfl/json/load.hpp>

#include <asio/io_context.hpp>
#include <rfl/json/write.hpp>

namespace pdft::rt {

namespace {

Status main(Config config) {
    log::loadConfig(config.logging);
    asio::io_context context(1);
    LoadContext ctx(context, std::move(config.modules), FactoryStorage::globalInstance());
    co_await ctx.loadAllModules();

    try {
        context.run();
        co_return unit;
    } catch (...) {
        co_return result::err("an unexpected exception in main loop");
    }
}

struct Modules {
    std::unordered_map<std::string, rfl::Generic> modules;
};

void listModules() {
    Modules res;

    for (const auto& [module, traits] : FactoryStorage::globalInstance().getModules()) {
        res.modules.emplace(module, traits->getConfig());
    }

    rfl::json::write(res, std::cout, rfl::json::pretty) << std::endl;
}

struct CliArgs {
    cli_opt<std::optional<std::string>>  //
        ::desc<"Path to the configuration file"> config_path;

    cli_opt<bool>                                      //
        ::desc<"Get default modules' configurations">  //
        ::shrt<'L'> list_modules;
};

}  // namespace

Result<int> start(
    const std::string& description, const std::string& version, std::span<const char*> argv) {
    if (!co_await parseCliArgs(argv, description, version)) {
        co_return 0;
    }

    auto cli_args = fromParsedCli<CliArgs>();
    if (!cli_args) {
        co_return result::err("Failed to parse CLI arguments: {}", cli_args.error());
    }

    if (cli_args->list_modules) {
        listModules();
        co_return 0;
    }

    auto config = rfl::json::load<Config, rfl::DefaultIfMissing>(
        cli_args->config_path->value_or(utils::expandHome("~/.config/pdft/config.json")));
    if (!config) {
        co_return result::err("Error while parsing configuration: {}", config.error()->what());
    }

    co_await main(*config);
    co_return 0;
}

}  // namespace pdft::rt

PDFT_REGISTER_CLI_ARGS(pdft::rt::CliArgs);
