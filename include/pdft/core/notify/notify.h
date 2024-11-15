#pragma once

#include <filesystem>

#include <asio/awaitable.hpp>

namespace pdft::notify {

struct Event {
    std::filesystem::path path;
};

class Notify {
 public:
    virtual ~Notify() = default;

    virtual void add(const std::filesystem::path& path) = 0;
    virtual void stop() = 0;
    virtual bool isStopped() const = 0;
    virtual asio::awaitable<Event> nextEvent() = 0;
};

}  // namespace pdft::notify
