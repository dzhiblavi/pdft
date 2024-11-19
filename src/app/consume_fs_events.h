#pragma once

#include "event_queue.h"

namespace app {

asio::awaitable<void> consumeFsEvents(EventQueue& queue, pdft::notify::Notify& notify);

}  // namespace app
