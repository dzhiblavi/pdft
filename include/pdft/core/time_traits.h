#pragma once

#include <chrono>

namespace pdft {

using SteadyClockType = std::chrono::steady_clock;
using SteadyTimePointType = typename SteadyClockType::time_point;
using SteadyDurationType = typename SteadyClockType::duration;

}  // namespace pdft
