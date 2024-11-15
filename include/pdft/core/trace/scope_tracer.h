#pragma once

#include "pdft/core/trace/time_traits.h"

namespace pdft::core::trace {

template <std::invocable<DurationType> Callback>
class ScopeTracer {
 public:
    explicit ScopeTracer(Callback callback)
        : scope_enter_(ClockType::now()), callback_(std::move(callback)) {}

    ~ScopeTracer() noexcept {
        try {
            std::move(callback_)(ClockType::now() - scope_enter_);
        } catch (...) {
            // ignore
        }
    }

 private:
    TimePointType scope_enter_;
    [[no_unique_address]] Callback callback_;
};

}  // namespace pdft::core::trace
