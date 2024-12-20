#pragma once

#include "pdft/core/time_traits.h"

namespace pdft::rt {

template <typename T>
struct Event {
    T value;
    SteadyTimePointType timestamp;
};

}  // namespace pdft::rt
