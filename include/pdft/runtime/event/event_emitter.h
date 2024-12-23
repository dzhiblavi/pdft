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
    using ActiveNode = utils::intrusive::ListNode<ActiveTag>;
    using UnsubNode = utils::intrusive::ListNode<UnsubscriberTag>;

    void clear() {
        for (auto& sub : unsubs_) {
            static_cast<ActiveNode&>(sub).unlink();
            static_cast<UnsubNode&>(sub).unlink();
        }
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
