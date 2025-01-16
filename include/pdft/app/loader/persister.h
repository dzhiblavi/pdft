#pragma once

#include "platform/core/result.h"

#include <ostream>

namespace pdft::app {

class Persister {
 public:
    virtual ~Persister() = default;

    virtual platform::Status save(std::ostream& os) const = 0;
    virtual platform::Status restore(std::istream& is) const = 0;
};

}  // namespace pdft::app
