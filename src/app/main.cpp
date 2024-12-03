#include "app.h"
#include "cli_args.h"

#include <asio/co_spawn.hpp>
#include <asio/detached.hpp>
#include <asio/io_context.hpp>

#include <iostream>
#include <rfl/Processors.hpp>
#include <rfl/yaml.hpp>

namespace app {

asio::awaitable<void> main(asio::io_context& context, Config config, const CliArgs& args) {
    setupLogging(config.logging);

    auto app = Application(context, std::move(config), args.state_mode);
    app.setLogger(pdft::core::log::global());
    co_await app.run(args.paths);
}

}  // namespace app

int main(int argc, const char** argv) {
    auto args = std::span(argv, argc);

    auto cli_args = app::parseCliArgs(args);
    if (!cli_args.has_value()) {
        std::cerr << std::format("Error while parsing CLI arguments: {}", cli_args.error())
                  << std::endl;
        return 1;
    }

    auto config = rfl::yaml::load<app::Config, rfl::DefaultIfMissing>(cli_args->config_path);
    if (!config) {
        std::cerr << std::format("Error while parsing configuration: {}", config.error()->what())
                  << std::endl;
        return 1;
    }

    asio::io_context context(1);
    asio::co_spawn(context, app::main(context, *config, *cli_args), asio::detached);

    try {
        context.run();
    } catch (...) {
        pdft::core::panic("an unexpected exception");
    }
    return 0;
}
