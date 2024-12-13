#pragma once

#include "pdft/core/module/detail/registerer.h"  // IWYU pragma: keep
#include "pdft/utils/anon_var_name.h"

#define PDFT_REGISTER_MODULE(Type)                            \
    [[maybe_unused]] const auto ANON_VAR(module_registerer) = \
        ::pdft::module::detail::ModuleRegister<Type>(#Type)

#define PDFT_REGISTER_FACTORY(Interface, Factory)              \
    [[maybe_unused]] const auto ANON_VAR(factory_registerer) = \
        ::pdft::module::detail::FactoryRegister<Interface, Factory>(#Factory)
