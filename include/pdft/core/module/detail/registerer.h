#pragma once

#include "pdft/core/module/factory_storage.h"

#include <string>

namespace pdft::module::detail {

template <IsModule T>
struct ModuleRegister {
    explicit ModuleRegister(const std::string& module_name) {
        FactoryStorage::globalInstance().template registerModule<T>(module_name);
    }
};

template <typename I, IsFactory<I> F>
requires IsModule<F>
struct FactoryRegister {
    explicit FactoryRegister(const std::string& module_name) {
        FactoryStorage::globalInstance().template registerFactoryModule<I, F>(module_name);
    }
};

}  // namespace pdft::module::detail
