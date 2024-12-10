#include "pdft/app/entrypoint.h"
#include "pdft/app/cli_args.h"
#include "pdft/app/palettes.h"
#include "pdft/app/version.h"

#include "pdft/core/module/register.h"
#include "pdft/utils/build_info.h"

#include <asio/co_spawn.hpp>
#include <asio/detached.hpp>

namespace pdft::app {

Entrypoint::Entrypoint(
    asio::io_context& io,
    std::span<const char*> argv,
    inject<PaletteStorage, "palettes"> palettes,
    inject<app::Application, "app"> app)
    : io_(io)
    , app_(std::move(app)) {
    auto args = parseCliArgs(argv);

    if (!args) {
        panic("failed to parse CLI args: {}", args.error());
    }

    paths_ = args->paths;
    palettes->add("skim", getSkimPalette());
}

void Entrypoint::start() {
    printAppInfo();
    asio::co_spawn(io_, app_->run(paths_), asio::detached);
}

void Entrypoint::printAppInfo() const {
    using namespace utils;
    using magic_enum::enum_name;

    log::info(this, "PDFT {}", getFullVersionString());

    log::info(
        this,
        "Platform={}, ASAN={}, TSAN={}, UBSAN={}",
        enum_name(getCurrentPlatform()),
        enum_name(isBuiltWithASAN()),
        enum_name(isBuiltWithTSAN()),
        enum_name(isBuiltWithUBSAN()));
}

}  // namespace pdft::app

PDFT_REGISTER_MODULE(pdft::app::Entrypoint);
