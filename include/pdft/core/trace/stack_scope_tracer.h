#pragma once

#include "pdft/core/trace/scope_tracer.h"

namespace pdft::core::trace {

class StackScopeTracerBase {
 public:
    explicit StackScopeTracerBase(StackScopeTracerBase* parent)
        : parent_(parent)
        , depth_(parent_ == nullptr ? 0 : parent->depth_ + 1) {}

    uint32_t depth() const {
        return depth_;
    }

    StackScopeTracerBase* parent() const {
        return parent_;
    }

 protected:
    StackScopeTracerBase* parent_ = nullptr;
    uint32_t depth_ = 0;
};

template <std::invocable<DurationType, StackScopeTracerBase&> Callback>
class StackScopeTracer : public StackScopeTracerBase {
 public:
    StackScopeTracer(Callback callback)
        : StackScopeTracerBase(nullptr)
        , plain_(ScopeCallback{.self = this})
        , callback_(std::move(callback)) {}

    StackScopeTracer(StackScopeTracer& parent, Callback callback)
        : StackScopeTracerBase(&parent)
        , plain_(ScopeCallback{.self = this})
        , callback_(std::move(callback)) {}

 private:
    struct ScopeCallback {
        StackScopeTracer* self;

        void operator()(DurationType duration) const {
            std::move(self->callback_)(duration, *self);
        }
    };

    ScopeTracer<ScopeCallback> plain_;
    [[no_unique_address]] Callback callback_;
};

}  // namespace pdft::core::trace
