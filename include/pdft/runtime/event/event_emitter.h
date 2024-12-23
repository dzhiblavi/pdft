#pragma once

#include "pdft/runtime/event/event.h"
#include "pdft/runtime/event/slot_base.h"

#include "pdft/utils/intrusive/list.h"

namespace pdft::rt {

template <typename T>
class EventEmitter {
 public:
    virtual ~EventEmitter() = default;

    virtual void stop() {}

    void emit(T event) {
        Event<T> evt{std::move(event), SteadyClockType::now()};
        emit(evt);
    }

 private:
    void clear() {
        for (auto& sub : unsubs_) {
            static_cast<utils::intrusive::ListNode<ActiveTag>&>(sub).unlink();
        }

        unsubs_.clear();
    }

    void plug(SlotBase<T>* slot) {
        subs_.push_back(*slot);
    }

    void unplug(SlotBase<T>* slot) {
        unsubs_.push_back(*slot);
    }

    void emit(const Event<T>& event) {
        clear();
        for (auto& sub : subs_) {
            sub.consume(event);
        }
        clear();
    }

    utils::intrusive::List<SlotBase<T>, ActiveTag> subs_;
    utils::intrusive::List<SlotBase<T>, UnsubscriberTag> unsubs_;

    friend struct SlotBase<T>;
};

}  // namespace pdft::rt
