#pragma once

#include "pdft/core/time_traits.h"

namespace pdft::event {

template <typename T>
struct Event {
    T value;
    SteadyTimePointType timestamp;
};

}  // namespace pdft::event
