#pragma once

#include "pdft/core/apc/apc_executor.h"
#include "pdft/core/io/console.h"

namespace pdft::core::apc {

class APCExecutorBase : public APCExecutor {
 public:
    explicit APCExecutorBase(std::shared_ptr<core::io::Console> console)
        : console_(std::move(console)) {}

    asio::awaitable<Result<APCResponse>> executeWithResponse(
        APC apc, io::OutBuffer& buffer) override;

 private:
    std::shared_ptr<core::io::Console> console_;
};

}  // namespace pdft::core::apc
