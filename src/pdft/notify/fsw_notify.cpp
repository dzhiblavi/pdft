#include "pdft/notify/impl/fsw_notify.h"

#include "pdft/core/assert.h"
#include "pdft/core/log.h"
#include "pdft/core/module/register.h"
#include "pdft/core/result/make/err.h"
#include "pdft/core/result/make/ok.h"

#include "pdft/utils/build_info.h"

#include <libfswatch/c/libfswatch.h>

#include <magic_enum.hpp>

namespace pdft::notify {

FswNotify::FswNotify(asio::any_io_executor executor) : channel_(executor, 10) {
    [[maybe_unused]] static auto _ = [] {
        PDFT_VERIFY(fsw_init_library() == FSW_OK);
        return 0;
    }();

    switch (utils::getCurrentPlatform()) {
        using enum utils::Platform;

        case Linux:
            // XXX FSW inotify support is kinda shit. When a file is updated
            // it starts sending events in an infinite stream.
            // handle_ = fsw_init_session(inotify_monitor_type);
            handle_ = fsw_init_session(poll_monitor_type);
            break;

        case MacOS:
            handle_ = fsw_init_session(system_default_monitor_type);
            break;

        case Posix:
            handle_ = fsw_init_session(poll_monitor_type);
            break;

        case Windows:
            handle_ = fsw_init_session(windows_monitor_type);
            break;

        case Unknown:
            panic("this platform is not supported by filesystem watcher");
            break;
    }

    PDFT_VERIFY(fsw_set_latency(handle_, 0.1) == FSW_OK);
    PDFT_VERIFY(fsw_set_callback(handle_, processEvents, this) == FSW_OK);

    fsw_event_type_filter filter;
    filter.flag = IsFile;
    PDFT_VERIFY(fsw_add_event_type_filter(handle_, filter) == FSW_OK);
    filter.flag = Updated;
    PDFT_VERIFY(fsw_add_event_type_filter(handle_, filter) == FSW_OK);

    // XXX Add a dummy path because FSW requires that at least one path
    // is added before polling is started
    add(".");

    fsw_thread_ = std::thread(monitorRunLoop, this);
}

FswNotify::~FswNotify() noexcept {
    if (!isStopped()) {
        stop();
    }

    if (fsw_thread_.joinable()) {
        fsw_thread_.join();
    }

    PDFT_VERIFY(fsw_destroy_session(handle_) == FSW_OK);
}

void FswNotify::stop() {
    should_exit_.store(true, std::memory_order_release);
    fsw_stop_monitor(handle_);
}

bool FswNotify::isStopped() const {
    return should_exit_.load(std::memory_order_relaxed);
}

void FswNotify::add(const std::filesystem::path& path) {
    fsw_stop_monitor(handle_);
    PDFT_VERIFY(fsw_add_path(handle_, path.c_str()) == FSW_OK);
}

AsyncResult<Event> FswNotify::nextEvent() {
    auto [ec, event] = co_await channel_.async_receive(AsioCoroAsTuple);

    if (ec) {
        co_return result::err(errorCodeToString(ec));
    }

    co_return result::ok(std::move(event));
}

void FswNotify::monitorRunLoop(FswNotify* self) {
    log::debug("start fswatch monitorRunLoop");

    while (!self->should_exit_.load(std::memory_order_acquire)) {
        fsw_start_monitor(self->handle_);
    }

    log::debug("end fswatch monitorRunLoop");
}

void FswNotify::processEvents(const fsw_cevent* const events, unsigned count, void* context) {
    auto* self = static_cast<FswNotify*>(context);
    log::trace("received {} filesystem events", count);

    for (unsigned i = 0; i < count && !self->isStopped(); ++i) {
        if (events[i].flags_num > 0 &&
            (events[i].flags[0] == NoOp || events[i].flags[0] == PlatformSpecific)) {
            continue;
        }

        log::trace("received a filesystem event for path {}", events[i].path);

        for (unsigned j = 0; j < events[i].flags_num; ++j) {
            log::trace(
                "filesystem event flag #{}: {}", j, magic_enum::enum_name(events[i].flags[j]));
        }

        self->channel_.async_send({}, Event{.path = events[i].path}, asio::use_future).get();
    }
}

}  // namespace pdft::notify

PDFT_REGISTER_MODULE(pdft::notify::FswNotify);
