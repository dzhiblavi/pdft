#pragma once

#include "pdft/core/log/with_logger.h"
#include "pdft/core/trace/stack_scope_tracer.h"

namespace pdft::core {

class Context;

class PushHandle {
 public:
    PushHandle(PushHandle&& rhs);
    ~PushHandle();

 private:
    PushHandle(
        core::trace::StackScopeTracerOwner tracer,
        core::trace::StackScopeTraceHandle previous,
        Context* context);

    core::trace::StackScopeTracerOwner tracer;
    core::trace::StackScopeTraceHandle previous;
    Context* context = nullptr;

    friend class Context;
};

class Context : public log::WithLogger {
 public:
    Context() = default;
    Context(trace::StackScopeTraceHandle trace_handle);

    Context(const Context&) = delete;
    Context& operator=(const Context&) = delete;

    void setTracer(trace::StackScopeTraceHandle tracer);
    PushHandle push(const char* label, std::source_location loc = std::source_location::current());

    template <typename F>
    decltype(auto) trace(
        const char* label, F&& func, std::source_location loc = std::source_location::current()) {
        auto _ = this->push(label, loc);
        return std::forward<F>(func)();
    }

 private:
    trace::StackScopeTraceHandle trace_handle_;

    friend class PushHandle;
};

}  // namespace pdft::core
