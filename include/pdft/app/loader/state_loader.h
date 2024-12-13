#pragma once

#include "pdft/core/result.h"

namespace pdft::app {

class StateLoader {
 public:
    virtual ~StateLoader() = default;

    virtual Status save() = 0;
    virtual Status load(const std::vector<std::string>& paths) = 0;
};

}  // namespace pdft::app
