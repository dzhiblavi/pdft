#pragma once

#include "platform/core/result.h"

namespace pdft::app {

class StateLoader {
 public:
    virtual ~StateLoader() = default;

    virtual platform::Status save() = 0;
    virtual platform::Status load(const std::vector<std::string>& paths) = 0;
};

}  // namespace pdft::app
