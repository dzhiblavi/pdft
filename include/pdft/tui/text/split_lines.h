#pragma once

#include <cppcoro/generator.hpp>

namespace pdft::tui::text {

inline cppcoro::generator<std::string_view> splitLines(std::string_view text) {
    size_t pos = 0;

    while (pos < text.size()) {
        size_t next_newline = text.find('\n', pos);
        size_t next_pos;
        uint32_t line_length;

        if (next_newline == std::string::npos) {
            line_length = text.size() - pos;
            next_pos = text.size();
        } else {
            line_length = next_newline - pos;
            next_pos = next_newline + 1;
        }

        co_yield text.substr(pos, line_length);
        pos = next_pos;
    }
}

}  // namespace pdft::tui::text
