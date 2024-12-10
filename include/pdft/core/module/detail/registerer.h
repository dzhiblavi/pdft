#pragma once

#include "pdft/core/module/factory_storage.h"

#include <string>

namespace pdft::module::detail {

template <typename T>
struct ModuleRegister {
    explicit ModuleRegister(const std::string& module_name) {
        FactoryStorage::globalInstance().template registerModule<T>(module_name);
    }
};

}  // namespace pdft::module::detail
