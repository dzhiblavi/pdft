#pragma once

// IWYU pragma: private, include "registerer.h"

#include "pdft/core/module/detail/make_traits.h"
#include "pdft/core/module/factory_storage.h"

namespace pdft::module::detail {

template <IsModule T>
struct ModuleRegister {
    explicit ModuleRegister(const std::string& module_name) {
        FactoryStorage::globalInstance().registerModule(module_name, moduleTraits<T>());
    }
};

template <typename I, IsFactory<I> F>
requires IsModule<F>
struct FactoryRegister {
    explicit FactoryRegister(const std::string& module_name) {
        auto* mod = moduleTraits<F>();
        auto* fac = factoryTraits<I, F>();
        FactoryStorage::globalInstance().registerFactoryModule(module_name, mod, fac);
    }
};

}  // namespace pdft::module::detail
