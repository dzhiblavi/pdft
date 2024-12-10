#pragma once

#include "pdft/core/module/factory_storage.h"
#include "pdft/utils/anon_var_name.h"

#include <string>

namespace pdft::detail {

template <typename T>
struct ModuleRegister {
    explicit ModuleRegister(const std::string& module_name) {
        FactoryStorage::globalInstance().template registerModule<T>(module_name);
    }
};

}  // namespace pdft::detail

#define PDFT_REGISTER_MODULE(Type)                            \
    [[maybe_unused]] const auto ANON_VAR(module_registerer) = \
        ::pdft::detail::ModuleRegister<Type>(#Type)
