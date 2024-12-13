#pragma once

#include "pdft/core/result.h"

#include <ostream>

namespace pdft::app {

class Persister {
 public:
    virtual ~Persister() = default;

    virtual Status save(std::ostream& os) const = 0;
    virtual Status restore(std::istream& is) const = 0;
};

}  // namespace pdft::app
