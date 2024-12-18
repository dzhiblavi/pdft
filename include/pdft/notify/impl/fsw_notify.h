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

class FswNotify : public Notify, public Module {
 public:
    FswNotify();
    ~FswNotify() noexcept override;

    void stop() override;
    void add(const std::filesystem::path& path) override;

 private:
    bool isStopped() const;
    static void monitorRunLoop(FswNotify* self);
    static void processEvents(const fsw_cevent* const events, unsigned count, void* context);

    std::atomic<bool> should_exit_ = false;
    FSW_HANDLE handle_ = nullptr;
    std::thread fsw_thread_;
};

}  // namespace pdft::notify
