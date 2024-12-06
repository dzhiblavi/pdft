#pragma once

#include "pdft/core/assert.h"
#include "pdft/core/context.h"
#include "pdft/core/log.h"
#include "pdft/core/trace/log_stack_trace.h"
#include "pdft/core/trace/stack_scope_tracer.h"
#include "pdft/utils/visit.h"

#include <asio/post.hpp>
#include <asio/thread_pool.hpp>
#include <asio/use_future.hpp>

#include <future>
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

    const R& get(TaskId id) {
        auto it = futures_.find(id);
        PDFT_VERIFY(it != futures_.end(), "unknown task id={}", id);
        return it->second.get();
    }

    template <typename Producer>
    requires std::is_invocable_r_v<R, Producer, Context&>
    bool post(TaskId id, Producer producer) {
        if (futures_.contains(id)) {
            return false;
        }

        auto future = asio::post(  //
            pool_,
            asio::use_future([this, p = std::move(producer)] {
                return processRequest(std::move(p));
            }));

        futures_.emplace(id, std::move(future));
        return true;
    }

 private:
    template <typename T>
    struct FutureValue {
        explicit FutureValue(std::future<T> future) : value_(std::move(future)) {}

        const T& get() {
            if (std::holds_alternative<std::future<T>>(value_)) {
                value_ = std::get<std::future<T>>(value_).get();
            }

            return std::get<T>(value_);
        }

     private:
        std::variant<std::future<T>, T> value_;
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
    std::mutex m_;
    asio::thread_pool pool_;
    std::unordered_map<TaskId, FutureValue<R>> futures_;
};

}  // namespace pdft::utils
