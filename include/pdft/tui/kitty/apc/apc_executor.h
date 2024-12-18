#pragma once

#include "pdft/tui/kitty/apc/command.h"

#include "pdft/io/buffer.h"

#include "pdft/core/context.h"
#include "pdft/core/result.h"

#include <asio/awaitable.hpp>

namespace pdft::tui {

struct APCExecutorConfig {
    static constexpr size_t TerminalMaxChunkSizeBytes = 4096;

    size_t max_chunk_size_bytes = TerminalMaxChunkSizeBytes;
};

class APCExecutor {
 public:
    virtual ~APCExecutor() = default;

    virtual void execute(Context& ctx, const Command& cmd, io::OutBuffer& buffer) = 0;
};

}  // namespace pdft::tui
