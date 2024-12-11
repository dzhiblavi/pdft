#pragma once

#include "pdft/notify/event.h"

#include <variant>

namespace pdft::app {

using Event = std::variant<char, notify::Event, int>;

}  // namespace pdft::app
