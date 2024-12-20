#pragma once

#include "pdft/runtime/event/event.h"

#include "pdft/utils/lock_free_stack.h"

namespace pdft::rt {

template <typename T>
class SlotBase : public utils::IntrusiveForwardListItem<SlotBase<T>> {
 public:
    virtual ~SlotBase() = default;
    virtual void consume(const Event<T>& event) = 0;
};

template <typename Receiver, typename T>
class Slot : public SlotBase<T> {
 public:
    using MemberPtr = void (Receiver::*)(const Event<T>&);

    Slot() = default;
    explicit Slot(Receiver* receiver) : receiver_(receiver) {}
    Slot(Receiver* receiver, MemberPtr member) : receiver_(receiver), member_(member) {}

    void attach(MemberPtr* member) {
        member_ = member;
    }

    void attach(Receiver* receiver, MemberPtr* member) {
        receiver_ = receiver;
        attach(member);
    }

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
