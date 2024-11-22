#pragma once

#include "pdft/core/apc/apc.h"
#include "pdft/core/io/buffer.h"
#include "pdft/core/result.h"

#include <asio/awaitable.hpp>

#include <expected>

namespace pdft::core::apc {

class APCExecutor {
 public:
    virtual ~APCExecutor() = default;

    virtual void execute(APC apc, io::OutBuffer& buffer) = 0;

    virtual asio::awaitable<Result<APCResponse>> executeWithResponse(
        APC apc, io::OutBuffer& buffer) = 0;
};

}  // namespace pdft::core::apc
