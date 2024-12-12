#pragma once

#include "pdft/core/module/factory.h"
#include "pdft/core/module/load/loader.h"

#include <unordered_map>

namespace pdft::module {

struct ModuleTraits {
    virtual void start(Module* self) const = 0;
    virtual Arc<Module> load(LoadContext& ctx, std::string name) const = 0;
};

struct FactoryTraits {
    virtual Arc<Module> create(Module* self) const = 0;
};

template <typename T>
concept Startable = requires(T& value) {
    { value.start() } -> std::convertible_to<void>;
};

template <typename T>
ModuleTraits* moduleTraits() {
    struct TraitsImpl : ModuleTraits {
        Arc<Module> load(LoadContext& ctx, std::string name) const override {
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
FactoryTraits* factoryTraits() {
    struct TraitsImpl : FactoryTraits {
        Arc<Module> create(Module* self) const override {
            F* factory = PDFT_VERIFY(dynamic_cast<F*>(self));
            return PDFT_VERIFY(std::dynamic_pointer_cast<Module>(factory->create()));
        }
    };

    static TraitsImpl traits;
    return &traits;
}

class FactoryStorage {
 public:
    using ModuleFactory = Arc<Module> (*)(LoadContext& ctx, std::string name);

    template <IsModule T>
    void registerModule(const std::string& cls) {
        PDFT_VERIFY(!modules_.contains(cls), "duplicate module {}", cls);
        modules_.emplace(cls, moduleTraits<T>());
    }

    template <typename I, IsFactory<I> F>
    requires IsModule<F>
    void registerFactoryModule(const std::string& cls) {
        registerModule<F>(cls);

        PDFT_VERIFY(!factories_.contains(cls), "duplicate factory {}", cls);
        factories_.emplace(cls, factoryTraits<I, F>());
    }

    ModuleTraits* getModuleTraits(const std::string& cls) {
        PDFT_VERIFY(modules_.contains(cls), "unknown module {}", cls);
        return modules_[cls];
    }

    FactoryTraits* getFactoryTraits(const std::string& cls) {
        PDFT_VERIFY(factories_.contains(cls), "unknown factory {}", cls);
        return factories_[cls];
    }

    static FactoryStorage& globalInstance() {
        static FactoryStorage storage;
        return storage;
    }

 private:
    FactoryStorage() = default;

    std::unordered_map<std::string, ModuleTraits*> modules_;
    std::unordered_map<std::string, FactoryTraits*> factories_;
};

}  // namespace pdft::module
