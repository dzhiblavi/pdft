#pragma once

#include "pdft/event/event.h"
#include "pdft/event/slot.h"

#include "pdft/utils/lock_free_stack.h"

namespace pdft::event {

template <typename T>
class EventEmitter {
 public:
    virtual ~EventEmitter() = default;

    virtual void stop() = 0;

    void subscribe(SlotBase<T>* slot) {
        subs_.push(*slot);
    }

    void emit(T event) {
        Event<T> evt{std::move(event), SteadyClockType::now()};
        emit(evt);
    }

 private:
    void emit(const Event<T>& event) {
        // XXX Subscribers cannot unsubscribe, so we can iterate safely
        subs_.forEach([&](auto&& sub) { sub.consume(event); });
    }

    utils::MPMCLockFreeStack<SlotBase<T>> subs_;
};

}  // namespace pdft::event
