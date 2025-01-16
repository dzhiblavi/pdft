#pragma once

#include "platform/core/result.h"

namespace pdft::app {

class Loader {
 public:
    virtual ~Loader() = default;

    virtual platform::Status load() = 0;
    virtual platform::Status save() = 0;
};

}  // namespace pdft::app
