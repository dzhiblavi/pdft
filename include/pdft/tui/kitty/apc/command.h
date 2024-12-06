#pragma once

#include <span>
#include <string>
#include <string_view>
#include <vector>

namespace pdft::tui {

enum class Compress {
    NoCompression,
    Compress,
    UseCompressed,
};

struct Command {
    std::string_view command;
    std::string_view payload = "";
    std::span<std::pair<std::string_view, std::string>> control_data;

    // Whether to encode payload in base64 format
    bool b64_encode = false;

    // Whether to compress payload
    Compress compress = Compress::NoCompression;
};

struct Response {
    std::string command;
    std::string payload;
    std::vector<std::pair<std::string, std::string>> control_data;
};

}  // namespace pdft::tui
