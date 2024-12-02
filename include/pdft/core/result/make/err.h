#pragma once

#include "pdft/core/result/type/error.h"

#include <expected>

namespace pdft::result {

inline auto err(Error error) {
    return std::unexpected(error);
}

}  // namespace pdft::result
