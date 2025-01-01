#pragma once

// IWYU pragma: private, include "registerer.h"

#include "pdft/runtime/module/detail/make_traits.h"
#include "pdft/runtime/module/factory_storage.h"

#include "pdft/core/result.h"

namespace pdft::rt::detail {

template <IsModule T>
struct ModuleRegister {
    explicit ModuleRegister(const std::string& module_name) {
        FactoryStorage::globalInstance().registerModule(module_name, moduleTraits<T>()) |
            result::unwrap;
    }
};

template <typename I, IsFactory<I> F>
requires IsModule<F>
struct FactoryRegister {
    explicit FactoryRegister(const std::string& module_name) {
        auto* mod = moduleTraits<F>();
        auto* fac = factoryTraits<I, F>();
        FactoryStorage::globalInstance().registerFactoryModule(module_name, mod, fac) |
            result::unwrap;
    }
};

}  // namespace pdft::rt::detail
