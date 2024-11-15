#pragma once

#include <span>
#include <string_view>

namespace pdft::core::terminal {

struct ApplciationProgrammingCommandView {
    std::string_view command;
    std::string_view payload_b64;
    std::span<std::pair<std::string_view, std::string>> control_data;
};

using APC = ApplciationProgrammingCommandView;

}  // namespace pdft::core::terminal
