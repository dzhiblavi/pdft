#pragma once

#include "pdft/core/result/type/status.h"

namespace pdft::app {

class Loader {
 public:
    virtual ~Loader() = default;

    virtual Status load() = 0;
    virtual Status save() = 0;
};

}  // namespace pdft::app
