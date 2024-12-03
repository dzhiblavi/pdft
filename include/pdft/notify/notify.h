#pragma once

#include "pdft/notify/event.h"

#include "pdft/core/awaitable.h"

#include <filesystem>

#include <asio/awaitable.hpp>

namespace pdft::notify {

class Notify {
 public:
    virtual ~Notify() = default;

    virtual void add(const std::filesystem::path& path) = 0;
    virtual void stop() = 0;
    virtual bool isStopped() const = 0;
    virtual AsyncResult<Event> nextEvent() = 0;
};

}  // namespace pdft::notify
