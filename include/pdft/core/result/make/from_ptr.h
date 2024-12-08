#pragma once

#include "pdft/core/result/make/none.h"
#include "pdft/core/result/type/maybe.h"

namespace pdft::result {

template <typename T>
Maybe<T*> fromPtr(T* pointer) {
    if (pointer == nullptr) {
        return none;
    }

    return pointer;
}

}  // namespace pdft::result
