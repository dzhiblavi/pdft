#pragma once

#include "pdft/core/assert.h"
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

#include <unordered_map>

namespace pdft::utils {

template <std::integral TaskId, typename R>
class BackgroundPool {
 public:
    explicit BackgroundPool(size_t num_threads) : pool_(num_threads) {}

    ~BackgroundPool() {
        requestStop();
        pool_.join();
    }

    void requestStop() {
        pool_.stop();
    }

    const R* get(TaskId id) {
        auto it = futures_.find(id);
        PDFT_VERIFY(it != futures_.end(), "unknown task id={}", id);
        return it->second.get();
    }

    template <typename Executor, typename Producer>
    requires std::is_invocable_r_v<R, Producer, Context&>
    bool post(Executor /*executor*/, TaskId id, Producer producer) {
        if (futures_.contains(id)) {
            return false;
        }

        // Promise<R> promise{executor};
        Promise<R> promise;
        auto future = promise.get_future();

        asio::post(pool_, [this, prom = std::move(promise), prod = std::move(producer)]() mutable {
            std::move(prom).set_value(processRequest(std::move(prod)));
        });

        futures_.emplace(id, std::move(future));
        return true;
    }

 private:
    template <typename T>
    struct FutureValue {
        explicit FutureValue(Future<T> future) : value_(std::move(future)) {}

        const T* get() {
            if (std::holds_alternative<Future<T>>(value_)) {
                Future<T> future = std::get<Future<T>>(std::move(value_));
                // value_ = co_await std::move(future).async_extract(asio::use_awaitable);
                value_ = std::move(future).get();
            }

            return &std::get<T>(value_);
        }

     private:
        std::variant<Future<T>, T> value_;
    };

    template <typename Producer>
    R processRequest(Producer producer) {
        auto tracer = trace::getGlobalPool().getTracer("bg_task");
        Context ctx;
        ctx.setTracer(tracer);
        ctx.setLogger(log_);

        R result = std::move(producer)(ctx);

        trace::logStackTrace(std::move(tracer), tracelog_);
        return result;
    }

    log::LoggerPtr log_ = log::loggerOrGlobal("bg");
    log::LoggerPtr tracelog_ = log::loggerOrGlobal("bg-trace");
    asio::thread_pool pool_;
    std::unordered_map<TaskId, FutureValue<R>> futures_;
};

}  // namespace pdft::utils
