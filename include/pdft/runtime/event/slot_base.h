#pragma once

#include "pdft/runtime/event/event.h"

#include <boost/intrusive/list_hook.hpp>

namespace pdft::rt {

struct ActiveTag;
struct UnsubscriberTag;

template <typename T>
class EventEmitter;

template <typename T>
struct SlotBase
    : boost::intrusive::list_base_hook<
          boost::intrusive::tag<ActiveTag>,
          boost::intrusive::link_mode<boost::intrusive::auto_unlink>>,
      boost::intrusive::list_base_hook<
          boost::intrusive::tag<UnsubscriberTag>,
          boost::intrusive::link_mode<boost::intrusive::auto_unlink>> {
    using ActiveHook = boost::intrusive::list_base_hook<
        boost::intrusive::tag<ActiveTag>,
        boost::intrusive::link_mode<boost::intrusive::auto_unlink>>;
    using UnsubHook = boost::intrusive::list_base_hook<
        boost::intrusive::tag<UnsubscriberTag>,
        boost::intrusive::link_mode<boost::intrusive::auto_unlink>>;

    virtual ~SlotBase() = default;
    virtual void consume(const Event<T>& event) = 0;

    void plug(EventEmitter<T>& emitter) {
        assert(!emitter_);
        emitter_ = &emitter;
        emitter_->plug(this);
    }

    void unplug() {
        if (!emitter_) {
            return;
        }

        emitter_->unplug(this);
    }

 protected:
    EventEmitter<T>* emitter_ = nullptr;
};

}  // namespace pdft::rt
