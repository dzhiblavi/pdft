#include "app.h"
#include "setup_logging.h"
#include "setup_signal_handlers.h"

#include <asio/co_spawn.hpp>
#include <asio/detached.hpp>
#include <asio/io_context.hpp>

#include <iostream>

namespace app {

asio::awaitable<void> main(asio::io_context& context, std::span<const char*> paths) {
    Application app(context);

    app.setup();
    co_await app.run(paths);
}

}  // namespace app

int main(int argc, const char** argv) {
    if (argc < 2) {
        std::cerr << "Usage: ./app <file>.pdf [<file>.pdf...]" << std::endl;
        return 1;
    }

    app::setupSignalHandlers();
    app::setupLogging();

    asio::io_context context(1);
    asio::co_spawn(context, app::main(context, std::span(argv, argc)), asio::detached);
    context.run();
    return 0;
}
