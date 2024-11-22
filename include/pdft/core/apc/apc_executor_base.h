#pragma once

#include "pdft/core/apc/apc_executor.h"
#include "pdft/core/io/console.h"

namespace pdft::core::apc {

struct Config {
    size_t max_chunk_size_bytes = 4096;
};

class APCExecutorBase : public APCExecutor {
 public:
    APCExecutorBase(const Config& config, std::shared_ptr<core::io::Console> console)
        : max_chunk_size_(config.max_chunk_size_bytes)
        , console_(std::move(console)) {}

    void execute(const APC& apc, io::OutBuffer& buffer) override;

    asio::awaitable<Result<APCResponse>> executeWithResponse(
        const APC& apc, io::OutBuffer& buffer) override;

 protected:
    virtual void writePrefix(io::OutBuffer& buffer) const = 0;
    virtual void writeSuffix(io::OutBuffer& buffer) const = 0;

 private:
    const size_t max_chunk_size_;
    std::shared_ptr<core::io::Console> console_;
};

}  // namespace pdft::core::apc
