#include "entrypoint.h"
#include "cli_args.h"
#include "palettes.h"
#include "version.h"

#include "pdft/core/module/register.h"
#include "pdft/utils/build_info.h"

namespace app {

Entrypoint::Entrypoint(
    asio::io_context& io,
    std::span<const char*> argv,
    inject<pdft::app::PaletteStorage, "palettes"> palettes,
    inject<app::Application, "app"> app)
    : io_(io)
    , app_(std::move(app)) {
    auto args = parseCliArgs(argv);

    if (!args) {
        pdft::panic("failed to parse CLI args: {}", args.error());
    }

    paths_ = args->paths;
    palettes->add("skim", getSkimPalette());
}

void Entrypoint::start() {
    printAppInfo();
    asio::co_spawn(io_, app_->run(paths_), asio::detached);
}

void Entrypoint::printAppInfo() const {
    using namespace pdft::utils;
    using magic_enum::enum_name;

    pdft::log::info(this, "PDFT {}", getFullVersionString());

    pdft::log::info(
        this,
        "Platform={}, ASAN={}, TSAN={}, UBSAN={}",
        enum_name(getCurrentPlatform()),
        enum_name(isBuiltWithASAN()),
        enum_name(isBuiltWithTSAN()),
        enum_name(isBuiltWithUBSAN()));
}

}  // namespace app

PDFT_REGISTER_MODULE(app::Entrypoint);
