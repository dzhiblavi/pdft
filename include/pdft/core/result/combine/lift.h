#pragma once

#include "pdft/core/result/make/err.h"
#include "pdft/core/result/make/ok.h"
#include "pdft/core/result/type/maybe.h"
#include "pdft/core/result/type/result.h"

namespace pdft::result {

namespace pipe {

struct Lift {
    Error error;

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
inline auto lift(Error err) {
    return pipe::Lift(std::move(err));
}

}  // namespace pdft::result
