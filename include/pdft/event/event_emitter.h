#pragma once

#include "pdft/event/event.h"

#include <functional>
#include <vector>

namespace pdft::event {

template <typename T>
class EventEmitter {
 public:
    using EventType = Event<T>;

    virtual ~EventEmitter() = default;

    virtual void stop() = 0;

    template <typename F>
    void subscribe(F&& func) {
        subs_.emplace_back(std::forward<F>(func));
    }

 protected:
    void emit(T event) {
        EventType evt{std::move(event), SteadyClockType::now()};
        emit(evt);
    }

 private:
    void emit(const EventType& event) {
        // XXX Subscribers cannot unsubscribe, so we can iterate safely
        for (auto& sub : subs_) {
            sub(event);
        }
    }

    std::vector<std::function<void(const EventType&)>> subs_;
};

}  // namespace pdft::event
