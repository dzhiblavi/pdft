#pragma once

#include "pdft/core/awaitable.h"
#include "pdft/core/context.h"
#include "pdft/core/future.h"
#include "pdft/core/log.h"
#include "pdft/core/trace/log_stack_trace.h"
#include "pdft/core/trace/stack_scope_tracer.h"

#include <asio/detached.hpp>
#include <asio/post.hpp>
#include <asio/thread_pool.hpp>
#include <asio/use_future.hpp>

namespace pdft::utils {

template <typename T>
struct FutureValue {
    Async<const T*> get() {
        if (std::holds_alternative<Future<T>>(value_)) {
            Future<T> future = std::get<Future<T>>(std::move(value_));
            value_ = co_await std::move(future).async_extract(asio::use_awaitable);
        }

        co_return &std::get<T>(value_);
    }

 private:
    explicit FutureValue(Future<T> future) : value_(std::move(future)) {}

    friend class BackgroundPool;

    std::variant<Future<T>, T> value_;
};

class BackgroundPool {
 public:
    explicit BackgroundPool(size_t num_threads);
    ~BackgroundPool();

    void requestStop();

    template <typename Executor, typename Producer>
    requires std::is_invocable_v<Producer, Context&>
    auto post(Executor executor, Producer producer) {
        using R = std::invoke_result_t<Producer, Context&>;

        Promise<R> promise{executor};
        auto future = promise.get_future();
        asio::post(pool_, [this, prom = std::move(promise), prod = std::move(producer)]() mutable {
            std::move(prom).set_value(processRequest(std::move(prod)));
        });
        return FutureValue{std::move(future)};
    }

 private:
    template <typename Producer>
    auto processRequest(Producer producer) {
        auto tracer = trace::getGlobalPool().getTracer("bg_task");
        Context ctx;
        ctx.setTracer(tracer);
        ctx.setLogger(log_);
        auto result = std::move(producer)(ctx);
        trace::logStackTrace(std::move(tracer), tracelog_);
        return result;
    }

    log::LoggerPtr log_ = log::loggerOrGlobal("bg");
    log::LoggerPtr tracelog_ = log::loggerOrGlobal("bg-trace");
    asio::thread_pool pool_;
};

}  // namespace pdft::utils
