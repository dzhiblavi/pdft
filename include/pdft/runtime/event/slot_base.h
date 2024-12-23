#pragma once

#include "pdft/runtime/event/event.h"

#include "pdft/utils/intrusive/node.h"

#include <cassert>

namespace pdft::rt {

struct ActiveTag;
struct UnsubscriberTag;

template <typename T>
class EventEmitter;

template <typename T>
struct SlotBase : utils::intrusive::ListNode<ActiveTag>,
                  utils::intrusive::ListNode<UnsubscriberTag> {
    virtual ~SlotBase() = default;
    virtual void consume(const Event<T>& event) = 0;

    void plug(EventEmitter<T>& emitter) {
        assert(emitter_ == nullptr);
        emitter_ = &emitter;
        emitter_->plug(this);
    }

    void unplug() {
        if (!emitter_) {
            return;
        }

        emitter_->unplug(this);
        emitter_ = nullptr;
    }

 protected:
    EventEmitter<T>* emitter_ = nullptr;
};

}  // namespace pdft::rt
