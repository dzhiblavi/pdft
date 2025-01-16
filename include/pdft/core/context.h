#pragma once

#include "platform/log/with_logger.h"

#include "platform/trace/stack_scope_tracer.h"

namespace pdft {

class Context;

class PushHandle {
 public:
    PushHandle(PushHandle&& rhs) noexcept;
    ~PushHandle();

    template <typename T>
    decltype(auto) operator|(T&& t) && {
        return std::forward<T>(t);
    }

 private:
    PushHandle() = default;

    PushHandle(
        platform::trace::StackScopeTracerOwner tracer,
        platform::trace::StackScopeTraceHandle previous,
        Context* context);

    platform::trace::StackScopeTracerOwner tracer;
    platform::trace::StackScopeTraceHandle previous;
    Context* context = nullptr;

    friend class Context;
};

class Context : public platform::log::WithLogger {
 public:
    Context() = default;
    Context(platform::trace::StackScopeTraceHandle trace_handle);

    Context(const Context&) = delete;
    Context& operator=(const Context&) = delete;

    void setTracer(platform::trace::StackScopeTraceHandle tracer);
    PushHandle push(const char* label, std::source_location loc = std::source_location::current());

 private:
    platform::trace::StackScopeTraceHandle trace_handle_;

    friend class PushHandle;
};

}  // namespace pdft
