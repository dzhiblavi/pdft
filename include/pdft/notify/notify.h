#pragma once

#include "pdft/notify/event.h"

#include "pdft/event/event_emitter.h"

#include <filesystem>

namespace pdft::notify {

class Notify : public event::EventEmitter<Event> {
 public:
    virtual ~Notify() = default;

    virtual void add(const std::filesystem::path& path) = 0;
};

}  // namespace pdft::notify
