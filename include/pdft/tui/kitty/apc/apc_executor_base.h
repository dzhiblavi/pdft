#pragma once

#include "pdft/tui/kitty/apc/apc_executor.h"

namespace pdft::tui {

class APCExecutorBase : public APCExecutor {
 public:
    explicit APCExecutorBase(APCExecutorConfig config = {})
        : max_chunk_size_(config.max_chunk_size_bytes) {}

    void execute(Context& ctx, const Command& cmd, io::OutBuffer& buffer) override;

 protected:
    virtual void writePrefix(io::OutBuffer& buffer) const = 0;
    virtual void writeSuffix(io::OutBuffer& buffer) const = 0;

 private:
    const size_t max_chunk_size_;
};

}  // namespace pdft::tui
