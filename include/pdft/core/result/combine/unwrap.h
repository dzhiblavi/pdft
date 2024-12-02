#pragma once

#include "pdft/core/result/type/result.h"

#include "pdft/core/assert.h"

#include <optional>

namespace pdft::result {

namespace pipe {

struct Unwrap {
    template <typename T>
    T Pipe(std::optional<T> optional) {
        PDFT_VERIFY(optional.has_value());
        return std::move(*optional);
    }

    template <typename T>
    T Pipe(Result<T> result) {
        PDFT_VERIFY(result.has_value());
        return std::move(*result);
    }
};

}  // namespace pipe

inline static constexpr pipe::Unwrap unwrap;

}  // namespace pdft::result
