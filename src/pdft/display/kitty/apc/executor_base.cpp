#include "pdft/display/kitty/apc/executor_base.h"
#include "pdft/display/kitty/apc/parse_response.h"

#include "pdft/utils/base64.h"
#include "pdft/utils/zlib.h"

namespace pdft::display::kitty::apc {

namespace {

std::string_view nextChunk(std::string_view& data, size_t max_chunk_size) {
    if (data.size() <= max_chunk_size) {
        auto result = data;
        data = "";
        return result;
    }

    auto result = data.substr(0, max_chunk_size);
    data = data.substr(max_chunk_size);
    return result;
}

template <typename Arg>
void writeControlData(core::io::OutBuffer& buffer, Arg&& value) {
    auto&& [k, v] = value;
    buffer << k << '=' << v;
}

template <typename Arg, typename... Args>
void writeControlData(core::io::OutBuffer& buffer, Arg&& arg, Args&&... rest) {
    writeControlData(buffer, std::forward<Arg>(arg));
    buffer << ',';
    writeControlData(buffer, std::forward<Args>(rest)...);
}

void writeControlData(const Command& apc, core::io::OutBuffer& buffer) {
    for (size_t i = 0; i < apc.control_data.size(); ++i) {
        auto&& [key, value] = apc.control_data[i];
        buffer << key << '=' << value;

        if (i + 1 < apc.control_data.size()) {
            buffer << ',';
        }
    }
}

std::string_view preparePayload(const Command& apc, std::string& encoded_payload) {
    std::string_view payload = apc.payload;

    if (apc.compress) {
        encoded_payload = utils::zlibCompress(payload);
        payload = encoded_payload;
    }

    if (apc.b64_encode) {
        encoded_payload = utils::encodeBase64(payload);
        payload = encoded_payload;
    }

    return payload;
}

}  // namespace

void ExecutorBase::execute(const Command& apc, core::io::OutBuffer& buffer) {
    std::string encoded_payload;
    auto payload = preparePayload(apc, encoded_payload);
    auto first_chunk = nextChunk(payload, max_chunk_size_);

    // Write the first chunk
    writePrefix(buffer);
    buffer << apc.command;
    writeControlData(apc, buffer);
    if (apc.compress) {
        buffer << ',';
        writeControlData(buffer, std::make_pair("o", "z"));
    }
    if (!payload.empty()) {
        buffer << ',';
        writeControlData(buffer, std::make_pair("m", "1"));
    }
    buffer << ';' << first_chunk;
    writeSuffix(buffer);

    // Write the rest of the data
    while (!payload.empty()) {
        auto chunk = nextChunk(payload, max_chunk_size_);

        writePrefix(buffer);
        buffer << apc.command;
        writeControlData(buffer, std::make_pair("m", payload.empty() ? "0" : "1"));
        buffer << ';' << chunk;
        writeSuffix(buffer);
    }
}

asio::awaitable<Result<Response>> ExecutorBase::executeWithResponse(
    const Command& apc, core::io::OutBuffer& buffer) {
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

}  // namespace pdft::display::kitty::apc
