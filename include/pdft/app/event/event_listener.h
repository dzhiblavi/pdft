#include "pdft/app/event/event.h"

#include "pdft/core/awaitable.h"

namespace pdft::app {

class EventListener {
 public:
    virtual ~EventListener() = default;

    virtual AsyncResult<Event> nextEvent() = 0;
};

}  // namespace pdft::app
