#pragma once

#include "pdft/runtime/module/detail/create_module.h"
#include "pdft/runtime/module/load.h"
#include "pdft/runtime/module/traits.h"

#include "pdft/core/factory.h"

#include <rfl/to_generic.hpp>

namespace pdft::rt::detail {

template <IsModule T>
ModuleTraits* moduleTraits() {
    struct TraitsImpl : ModuleTraits {
        Result<Arc<Module>> createVia(
            LoadContext& ctx, const ModuleConfig& config, const std::string& name) const override {
            return createModule<T>(ctx, config, name);
        }

        rfl::Generic getConfig() const override {
            if constexpr (HasConfig<T>) {
                using Config = typename T::Config;
                Config c;
                return rfl::to_generic(c);
            } else {
                return rfl::Generic::Null;
            }
        }
    };

    static TraitsImpl traits;
    return &traits;
}

template <typename I, IsFactory<I> F>
requires IsModule<F>
FactoryTraits* factoryTraits() {
    struct TraitsImpl : FactoryTraits {
        Result<Arc<Module>> create(Module* self) const override {
            F* factory = PDFT_VERIFY(dynamic_cast<F*>(self), "inconsistent factory traits");
            return PDFT_VERIFY(
                std::dynamic_pointer_cast<Module>(factory->create()),
                "stuff created by this factory is not even a module");
        }
    };

    static TraitsImpl traits;
    return &traits;
}

}  // namespace pdft::rt::detail
