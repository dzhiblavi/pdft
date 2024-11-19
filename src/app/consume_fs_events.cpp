#include "consume_fs_events.h"

namespace app {

asio::awaitable<void> consumeFsEvents(EventQueue& queue, pdft::notify::Notify& notify) {
    while (!notify.isStopped()) {
        auto event = co_await notify.nextEvent();
        co_await queue.async_send({}, event);
    }
}

}  // namespace app
