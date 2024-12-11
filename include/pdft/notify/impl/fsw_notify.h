#pragma once

#include "pdft/notify/notify.h"

#include "pdft/core/module/module.h"

#include <asio/experimental/concurrent_channel.hpp>
#include <asio/use_awaitable.hpp>
#include <asio/use_future.hpp>

#include <libfswatch/c/cevent.h>
#include <libfswatch/c/libfswatch_types.h>

#include <thread>

namespace pdft::notify {

class FswNotify : public notify::Notify, public Module {
 public:
    explicit FswNotify(asio::any_io_executor executor);
    ~FswNotify() noexcept override;

    void stop() override;
    bool isStopped() const override;
    void add(const std::filesystem::path& path) override;
    AsyncResult<Event> nextEvent() override;

 private:
    using EventChannel = asio::experimental::
        concurrent_channel<asio::any_io_executor, void(asio::error_code, Event)>;

    static void monitorRunLoop(FswNotify* self);
    static void processEvents(const fsw_cevent* const events, unsigned count, void* context);

    std::atomic<bool> should_exit_ = false;
    FSW_HANDLE handle_ = nullptr;
    EventChannel channel_;
    std::thread fsw_thread_;
};

}  // namespace pdft::notify
