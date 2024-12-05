#pragma once

#include "pdft/core/log/with_logger.h"
#include "pdft/core/trace/stack_scope_tracer.h"

namespace pdft {

class Context;

class PushHandle {
 public:
    PushHandle(PushHandle&& rhs);
    ~PushHandle();

    template <typename T>
    decltype(auto) operator|(T&& t) && {
        return std::forward<T>(t);
    }

 private:
    PushHandle() = default;

    PushHandle(
        trace::StackScopeTracerOwner tracer,
        trace::StackScopeTraceHandle previous,
        Context* context);

    trace::StackScopeTracerOwner tracer;
    trace::StackScopeTraceHandle previous;
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

 private:
    trace::StackScopeTraceHandle trace_handle_;

    friend class PushHandle;
};

}  // namespace pdft
