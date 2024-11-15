#pragma once

#include <chrono>

namespace pdft::core::trace {

using ClockType = std::chrono::steady_clock;
using TimePointType = typename ClockType::time_point;
using DurationType = typename ClockType::duration;

}  // namespace pdft::core::trace
