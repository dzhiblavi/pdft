#pragma once

#include "pdft/core/module/detail/registerer.h"
#include "pdft/utils/anon_var_name.h"

#define PDFT_REGISTER_MODULE(Type)                            \
    [[maybe_unused]] const auto ANON_VAR(module_registerer) = \
        ::pdft::module::detail::ModuleRegister<Type>(#Type)
