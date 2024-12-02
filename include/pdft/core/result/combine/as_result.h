#pragma once

#include "pdft/core/result/make/err.h"
#include "pdft/core/result/type/result.h"

#include <optional>

namespace pdft::result {

namespace pipe {

struct AsResult {
    std::string error;

    template <typename T>
    Result<T> Pipe(std::optional<T> opt) {
        if (opt.has_value()) {
            return ok(std::move(*opt));
        }

        return err(std::move(error));
    }
};

}  // namespace pipe

inline auto asResult(std::string error) {
    return pipe::AsResult(std::move(error));
}

}  // namespace pdft::result
