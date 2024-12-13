#pragma once

#include "pdft/core/assert.h"
#include "pdft/core/rc.h"

#include "pdft/utils/fixed_string.h"

namespace pdft::module::detail {

struct InjectedTag {};

template <typename T, utils::FixedString moduleName>
struct Injected : InjectedTag {
    static constexpr utils::FixedString Name = moduleName;

    /* implicit */ Injected(Arc<T> mod) : value_(std::move(mod)) {}

    template <typename U>
    explicit Injected(Arc<U> mod) {
        value_ = PDFT_VERIFY(
            std::dynamic_pointer_cast<T>(std::move(mod)),
            "incompatible injected value (module {})",
            Name.c_str());
    }

    operator Arc<T>() const {
        return value_;
    }

    T* operator->() {
        return value_.get();
    }

    const T* operator->() const {
        return value_.get();
    }

 private:
    Arc<T> value_;
};

}  // namespace pdft::module::detail
