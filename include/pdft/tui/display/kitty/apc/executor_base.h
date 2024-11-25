#pragma once

#include "pdft/core/io/console.h"
#include "pdft/tui/display/kitty/apc/executor.h"

namespace pdft::tui::display::kitty::apc {

struct Config {
    size_t max_chunk_size_bytes = 4096;
};

class ExecutorBase : public Executor {
 public:
    ExecutorBase(const Config& config, std::shared_ptr<core::io::Console> console)
        : max_chunk_size_(config.max_chunk_size_bytes)
        , console_(std::move(console)) {}

    void execute(const Command& apc, core::io::OutBuffer& buffer) override;

    asio::awaitable<Result<Response>> executeWithResponse(
        const Command& apc, core::io::OutBuffer& buffer) override;

 protected:
    virtual void writePrefix(core::io::OutBuffer& buffer) const = 0;
    virtual void writeSuffix(core::io::OutBuffer& buffer) const = 0;

 private:
    const size_t max_chunk_size_;
    std::shared_ptr<core::io::Console> console_;
};

}  // namespace pdft::tui::display::kitty::apc
