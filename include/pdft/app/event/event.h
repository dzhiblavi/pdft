#pragma once

#include "pdft/notify/event.h"

#include "pdft/core/time_traits.h"

#include <variant>

namespace pdft::app {

using EventType = std::variant<char, notify::Event, int>;

struct Event {
    TimePointType timestamp;
    EventType event;
};

}  // namespace pdft::app
