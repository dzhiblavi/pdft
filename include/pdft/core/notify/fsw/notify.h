#pragma once

#include "pdft/core/assert.h"
#include "pdft/core/log/log.h"
#include "pdft/core/notify/notify.h"

#include <asio/experimental/channel.hpp>
#include <asio/use_awaitable.hpp>
#include <asio/use_future.hpp>

#include <libfswatch/c/libfswatch.h>

#include <thread>

namespace pdft::notify::fsw {

template <typename Executor>
class Notify : public notify::Notify {
    using EventChannel = asio::experimental::channel<Executor, void(asio::error_code, Event)>;

 public:
    explicit Notify(const Executor& executor) : channel_(executor, 10) {
        static auto _ = [] {
            PDFT_VERIFY(fsw_init_library() == FSW_OK);
            return 0;
        }();

        handle_ = fsw_init_session(system_default_monitor_type);
        PDFT_VERIFY(fsw_set_latency(handle_, 0.1) == FSW_OK);
        PDFT_VERIFY(fsw_set_callback(handle_, processEvents, this) == FSW_OK);

        fsw_thread_ = std::thread(monitorRunLoop, this);
    }

    ~Notify() {
        if (!isStopped()) {
            stop();
        }

        if (fsw_thread_.joinable()) {
            fsw_thread_.join();
        }

        PDFT_VERIFY(fsw_destroy_session(handle_) == FSW_OK);
    }

    void stop() override {
        should_exit_.store(true, std::memory_order_relaxed);
        fsw_stop_monitor(handle_);
    }

    bool isStopped() const override {
        return should_exit_.load(std::memory_order_relaxed);
    }

    void add(const std::filesystem::path& path) override {
        fsw_stop_monitor(handle_);
        PDFT_VERIFY(fsw_add_path(handle_, path.c_str()) == FSW_OK);
    }

    asio::awaitable<Event> nextEvent() override {
        co_return co_await channel_.async_receive(asio::use_awaitable);
    }

 private:
    static void monitorRunLoop(Notify* self) {
        core::log::global()->info("start fswatch monitorRunLoop");

        while (!self->should_exit_.load(std::memory_order_relaxed)) {
            fsw_start_monitor(self->handle_);
        }

        core::log::global()->info("end fswatch monitorRunLoop");
    }

    static void processEvents(const fsw_cevent* const events, unsigned count, void* context) {
        auto* self = static_cast<Notify*>(context);
        core::log::global()->info("received {} events", count);

        for (unsigned i = 0; i < count; ++i) {
            core::log::global()->info("received an event for path {}", events[i].path);

            self->channel_
                .async_send(
                    {},
                    Event{
                        .path = events[i].path,
                    },
                    asio::use_future)
                .get();
        }
    }

    std::atomic<bool> should_exit_ = false;
    FSW_HANDLE handle_ = nullptr;
    EventChannel channel_;
    std::thread fsw_thread_;
};

}  // namespace pdft::notify::fsw
