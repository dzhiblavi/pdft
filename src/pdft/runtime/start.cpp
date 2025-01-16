#include "pdft/runtime/start.h"
#include "pdft/runtime/config.h"

#include "platform/cli/cli.h"
#include "platform/cli/parse.h"
#include "platform/cli/parsed.h"
#include "platform/cli/register.h"
#include "platform/log/load.h"
#include "platform/module/load.h"
#include "platform/utils/home_dir.h"

#include <rfl/Processors.hpp>
#include <rfl/json/load.hpp>

#include <asio/io_context.hpp>
#include <rfl/json/write.hpp>

namespace pdft::rt {

using namespace platform;

namespace {

Status main(Config config) {
    log::loadConfig(config.logging);
    asio::io_context context(1);
    platform::rt::LoadContext ctx(
        context, std::move(config.modules), platform::rt::FactoryStorage::globalInstance());
    co_await ctx.loadAllModules();

    try {
        context.run();
        co_return result::unit;
    } catch (...) {
        co_return result::makeError<std::string>("an unexpected exception in main loop");
    }
}

struct Modules {
    std::unordered_map<std::string, rfl::Generic> modules;
};

void listModules() {
    Modules res;

    for (const auto& [module, traits] :
         platform::rt::FactoryStorage::globalInstance().getModules()) {
        res.modules.emplace(module, traits->getConfig());
    }

    rfl::json::write(res, std::cout, rfl::json::pretty) << std::endl;
}

struct CliArgs {
    platform::rt::cli_opt<std::optional<std::string>>  //
        ::desc<"Path to the configuration file"> config_path;

    platform::rt::cli_opt<bool>                        //
        ::desc<"Get default modules' configurations">  //
        ::shrt<'L'> list_modules;
};

}  // namespace

Result<int> start(
    const std::string& description, const std::string& version, std::span<const char*> argv) {
    if (!co_await platform::rt::parseCliArgs(argv, description, version)) {
        co_return 0;
    }

    auto cli_args = platform::rt::fromParsedCli<CliArgs>();
    if (!cli_args) {
        co_return result::makeError(
            std::format("Failed to parse CLI arguments: {}", cli_args.error<std::string>()));
    }

    if (cli_args->list_modules) {
        listModules();
        co_return 0;
    }

    auto config = rfl::json::load<Config, rfl::DefaultIfMissing>(
        cli_args->config_path->value_or(utils::expandHome("~/.config/pdft/config.json")));
    if (!config) {
        co_return result::makeError(
            std::format("Error while parsing configuration: {}", config.error()->what()));
    }

    co_await main(*config);
    co_return 0;
}

}  // namespace pdft::rt

PLATFORM_REGISTER_CLI_ARGS(pdft::rt::CliArgs);
