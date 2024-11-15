#pragma once

#include "pdft/core/io/buffer.h"
#include "pdft/view/placement.h"

namespace pdft::view {

class Renderer {
 public:
    virtual ~Renderer() = default;

    virtual void render(core::OutBuffer& buffer, const std::vector<Placement>& layout) = 0;
};

}  // namespace pdft::view
