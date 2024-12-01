#pragma once

#include "pdft/tui/display/kitty/apc/executor.h"

namespace pdft::tui::display::kitty::apc {

struct Config {
    size_t max_chunk_size_bytes = 4096;
};

class ExecutorBase : public Executor {
 public:
    ExecutorBase(const Config& config = {}) : max_chunk_size_(config.max_chunk_size_bytes) {}

    void execute(Context& ctx, const Command& cmd, core::io::OutBuffer& buffer) override;

    asio::awaitable<Result<Response>> executeWithResponse(
        Context& ctx, const Command& cmd, core::io::OutBuffer& buffer) override;

 protected:
    virtual void writePrefix(core::io::OutBuffer& buffer) const = 0;
    virtual void writeSuffix(core::io::OutBuffer& buffer) const = 0;

 private:
    const size_t max_chunk_size_;
};

}  // namespace pdft::tui::display::kitty::apc
