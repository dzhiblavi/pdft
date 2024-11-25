#pragma once

#include <cppcoro/generator.hpp>

#include <string_view>

namespace pdft::tui::text {

inline cppcoro::generator<std::string_view> splitToWidth(std::string_view text, size_t width) {
    while (text.size() > 0) {
        size_t len = std::min(text.size(), width);
        auto substr = text.substr(0, len);
        text = text.substr(len);
        co_yield substr;
    }
}

}  // namespace pdft::tui::text
