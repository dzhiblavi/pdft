#pragma once

#include "pdft/notify/event.h"

#include "platform/event/event_emitter.h"

#include <filesystem>

namespace pdft::notify {

class Notify : public platform::rt::EventEmitter<Event> {
 public:
    virtual ~Notify() = default;

    virtual void add(const std::filesystem::path& path) = 0;
};

}  // namespace pdft::notify
