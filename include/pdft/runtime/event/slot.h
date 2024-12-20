#pragma once

#include "pdft/runtime/event/slot_base.h"

namespace pdft::rt {

template <typename Receiver, typename T>
struct Slot : SlotBase<T> {
    using MemberPtr = void (Receiver::*)(const Event<T>&);

    Slot(Receiver* receiver, MemberPtr member) : receiver_(receiver), member_(member) {}

    void consume(const Event<T>& event) override {
        if (!receiver_ || !member_) {
            return;
        }

        (receiver_->*member_)(event);
    }

 private:
    Receiver* receiver_ = nullptr;
    MemberPtr member_ = nullptr;
};

}  // namespace pdft::rt
