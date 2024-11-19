#include "keyboard_consumer.h"

namespace app {

KeyboardConsumer::KeyboardConsumer(std::shared_ptr<pdft::core::io::Console> console)
    : console_(std::move(console)) {}

asio::awaitable<void> KeyboardConsumer::consumeInto(EventQueue& queue) {
    while (!should_stop_) {
        auto key = co_await console_->read(1);
        co_await queue.async_send({}, KeyPressEvent{.key = key[0]});
    }
}

void KeyboardConsumer::stop() {
    should_stop_ = true;
}

}  // namespace app
