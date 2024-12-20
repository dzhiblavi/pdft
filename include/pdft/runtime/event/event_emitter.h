#pragma once

#include "pdft/runtime/event/event.h"
#include "pdft/runtime/event/slot_base.h"

#include <boost/intrusive/list.hpp>

namespace pdft::rt {

template <typename T>
class EventEmitter {
 public:
    virtual ~EventEmitter() = default;

    virtual void stop() = 0;

    void emit(T event) {
        Event<T> evt{std::move(event), SteadyClockType::now()};
        emit(evt);
    }

 private:
    using ActiveHook = typename SlotBase<T>::ActiveHook;
    using UnsubHook = typename SlotBase<T>::UnsubHook;

    void plug(SlotBase<T>* slot) {
        subs_.push_back(*slot);
    }

    void unplug(SlotBase<T>* slot) {
        unsubs_.push_back(*slot);
    }

    void emit(const Event<T>& event) {
        for (auto& sub : subs_) {
            sub.consume(event);
        }

        for (auto& sub : unsubs_) {
            static_cast<ActiveHook&>(sub).unlink();
        }

        unsubs_.clear();
    }

    boost::intrusive::list<
        SlotBase<T>,
        boost::intrusive::base_hook<ActiveHook>,
        boost::intrusive::constant_time_size<false>>
        subs_;

    boost::intrusive::list<
        SlotBase<T>,
        boost::intrusive::base_hook<UnsubHook>,
        boost::intrusive::constant_time_size<false>>
        unsubs_;

    friend struct SlotBase<T>;
};

}  // namespace pdft::rt
