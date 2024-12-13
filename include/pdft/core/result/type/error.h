#pragma once

#include <system_error>

#include <magic_enum.hpp>
#include <rfl/type_name_t.hpp>

namespace pdft {

using Error = std::string;

inline Error errorCodeToString(std::error_code ec) {
    return std::format("{}: {}", ec.category().name(), ec.category().message(ec.value()));
}

}  // namespace pdft
