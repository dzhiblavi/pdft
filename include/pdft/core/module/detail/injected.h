#pragma once

#include "pdft/core/rc.h"

#include "pdft/utils/fixed_string.h"

namespace pdft::module::detail {

struct InjectedTag {};

template <typename T, utils::FixedString moduleName>
struct Injected : InjectedTag {
    using InjectedType = T;

    static constexpr utils::FixedString Name = moduleName;

    /* implicit */ Injected(Arc<T> mod) : value_(std::move(mod)) {}

    operator Arc<T>() const {
        return value_;
    }

    operator Arc<T>() && {
        return std::move(value_);
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
