#include "pdft/app/entrypoint.h"
#include "pdft/app/version.h"

#include "platform/module/register.h"
#include "platform/trace/log_stack_trace.h"

#include "pdft/utils/build_info.h"

#include <asio/co_spawn.hpp>
#include <asio/detached.hpp>

namespace pdft::app {

using namespace platform;

Entrypoint::Entrypoint(asio::io_context& io, platform::Arc<app::Application> app)
    : io_(io)
    , app_(std::move(app)) {}

void Entrypoint::start() {
    printAppInfo();
    asio::co_spawn(io_, run(), asio::detached);
}

platform::Async<void> Entrypoint::run() {
    auto tracer = platform::trace::makeTracer("entrypoint");
    platform::trace::Context ctx(tracer);
    co_await app_->run(ctx);
    platform::trace::logStackTrace(std::move(tracer));
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

PLATFORM_REGISTER_MODULE(pdft::app::Entrypoint);
