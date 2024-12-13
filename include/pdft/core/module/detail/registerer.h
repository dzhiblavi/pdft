#pragma once

// IWYU pragma: private, include "registerer.h"

#include "pdft/core/module/detail/factory_storage.h"
#include "pdft/core/module/factory.h"

#include "pdft/core/module/load/detail/make_module.h"

namespace pdft::module::detail {

template <typename T>
concept Startable = requires(T& value) {
    { value.start() } -> std::convertible_to<void>;
};

template <IsModule T>
ModuleTraits* moduleTraits() {
    struct TraitsImpl : ModuleTraits {
        Result<Arc<Module>> load(LoadContext& ctx, std::string name) const override {
            return makeModule<T>(ctx, std::move(name));
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
