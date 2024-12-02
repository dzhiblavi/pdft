#pragma once

#include "pdft/core/result/make/err.h"
#include "pdft/core/result/type/result.h"

namespace pdft::result {

namespace pipe {

struct Lift {
    std::string error;

    template <typename T>
    Result<T> pipe(Maybe<T> opt) && {
        if (opt.has_value()) {
            return ok(std::move(*opt));
        }

        return err(std::move(error));
    }
};

}  // namespace pipe

// Maybe<T> -> Result<T>
inline auto lift(std::string error) {
    return pipe::Lift(std::move(error));
}

}  // namespace pdft::result
