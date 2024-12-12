#pragma once

#include "pdft/tui/kitty/apc/command.h"

#include "pdft/core/context.h"
#include "pdft/core/io/buffer.h"
#include "pdft/core/result.h"

#include <asio/awaitable.hpp>

namespace pdft::tui {

struct APCExecutorConfig {
    size_t max_chunk_size_bytes = 4096;
};

class APCExecutor {
 public:
    virtual ~APCExecutor() = default;

    virtual void execute(Context& ctx, const Command& cmd, io::OutBuffer& buffer) = 0;
};

}  // namespace pdft::tui
