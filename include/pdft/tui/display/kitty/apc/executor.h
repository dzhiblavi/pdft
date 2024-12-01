#pragma once

#include "pdft/core/context.h"
#include "pdft/core/io/buffer.h"
#include "pdft/core/result.h"
#include "pdft/tui/display/kitty/apc/command.h"

#include <asio/awaitable.hpp>

namespace pdft::tui::display::kitty::apc {

class Executor {
 public:
    virtual ~Executor() = default;

    virtual void execute(Context& ctx, const Command& cmd, core::io::OutBuffer& buffer) = 0;

    virtual asio::awaitable<Result<Response>> executeWithResponse(
        Context& ctx, const Command& cmd, core::io::OutBuffer& buffer) = 0;
};

}  // namespace pdft::tui::display::kitty::apc
