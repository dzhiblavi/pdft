#pragma once

#include "pdft/core/assert.h"
#include "pdft/core/time_traits.h"

namespace pdft::trace {

template <std::invocable<SteadyDurationType> Callback>
class ScopeTracer {
 public:
    explicit ScopeTracer(Callback callback)
        : scope_enter_(SteadyClockType::now())
        , callback_(std::move(callback)) {}

    ~ScopeTracer() noexcept {
        try {
            std::move(callback_)(SteadyClockType::now() - scope_enter_);
        } catch (...) {
            panic("unhandled ScopeTracer callback exception");
        }
    }

 private:
    SteadyTimePointType scope_enter_;
    [[no_unique_address]] Callback callback_;
};

}  // namespace pdft::trace
