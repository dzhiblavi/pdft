#pragma once

#include "pdft/core/module/detail/make_module.h"
#include "pdft/core/module/factory.h"
#include "pdft/core/module/traits.h"

namespace pdft::module::detail {

template <typename T>
concept Startable = requires(T& value) {
    { value.start() } -> std::convertible_to<void>;
};

template <IsModule T>
ModuleTraits* moduleTraits() {
    struct TraitsImpl : ModuleTraits {
        Result<Arc<Module>> load(LoadContext& ctx, std::string name) const override {
            return loadModule<T>(ctx, std::move(name));
        }

        void start(Module* self) const override {
            if constexpr (Startable<T>) {
                PDFT_VERIFY(dynamic_cast<T*>(self))->start();
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
                "inconsistent factory traits");
        }
    };

    static TraitsImpl traits;
    return &traits;
}

}  // namespace pdft::module::detail
