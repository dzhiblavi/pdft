#include "pdft/core/context.h"

namespace pdft {

PushHandle::PushHandle(
    trace::StackScopeTracerOwner tracer, trace::StackScopeTraceHandle previous, Context* context)
    : tracer(std::move(tracer))
    , previous(previous)
    , context(context) {}

PushHandle::PushHandle(PushHandle&& rhs)
    : tracer(std::move(rhs.tracer))
    , previous(rhs.previous)
    , context(std::exchange(rhs.context, nullptr)) {}

PushHandle::~PushHandle() {
    if (context == nullptr) {
        return;
    }

    context->trace_handle_ = previous;
}

Context::Context(trace::StackScopeTraceHandle trace_handle) : trace_handle_(trace_handle) {}

void Context::setTracer(trace::StackScopeTraceHandle tracer) {
    trace_handle_ = tracer;
}

PushHandle Context::push(const char* label, std::source_location loc) {
    if (!trace_handle_.isValid()) {
        return {};
    }

    PushHandle handle(trace_handle_.addChild(label, loc), this->trace_handle_, this);
    this->trace_handle_ = handle.tracer;
    return handle;
}

}  // namespace pdft
