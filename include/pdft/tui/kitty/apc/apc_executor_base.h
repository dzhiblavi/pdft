#pragma once

#include "pdft/tui/kitty/apc/apc_executor.h"

namespace pdft::tui {

struct APCExecutorConfig {
    size_t max_chunk_size_bytes = 4096;
};

class APCExecutorBase : public APCExecutor {
 public:
    APCExecutorBase(const APCExecutorConfig& config = {})
        : max_chunk_size_(config.max_chunk_size_bytes) {}

    void execute(Context& ctx, const Command& cmd, io::OutBuffer& buffer) override;

 protected:
    virtual void writePrefix(io::OutBuffer& buffer) const = 0;
    virtual void writeSuffix(io::OutBuffer& buffer) const = 0;

 private:
    const size_t max_chunk_size_;
};

}  // namespace pdft::tui
