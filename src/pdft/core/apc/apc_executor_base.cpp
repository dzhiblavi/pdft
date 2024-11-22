#include "pdft/core/apc/apc_executor_base.h"
#include "pdft/core/apc/parse_response.h"

namespace pdft::core::apc {

asio::awaitable<std::expected<APCResponse, std::string>> APCExecutorBase::executeWithResponse(
    APC apc, io::OutBuffer& buffer) {
    execute(std::move(apc), buffer);

    // Flush command and receive the response
    co_await console_->write(buffer);

    // At least 2 chars for escape codes + 2 chars for command (\033 _G \033\)
    std::stringstream in;
    std::string instr;
    co_await console_->read(5, instr);

    in << instr;

    while (in.view().back() != '\\') {
        co_await console_->read(1, instr);
        in << instr;
    }

    co_return parseResponse(in.view());
}

}  // namespace pdft::core::apc
