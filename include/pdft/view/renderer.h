#pragma once

#include "pdft/core/io/buffer.h"
#include "pdft/view/placement.h"

namespace pdft::view {

class Renderer {
 public:
    virtual ~Renderer() = default;

    virtual void render(core::io::OutBuffer& buffer, const PagePlacement& placement) = 0;
    virtual void render(core::io::OutBuffer& buffer, const TextPlacement& placement) = 0;
};

}  // namespace pdft::view
