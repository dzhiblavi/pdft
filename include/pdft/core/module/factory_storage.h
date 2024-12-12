#pragma once

#include "pdft/core/module/load/loader.h"

#include <unordered_map>

namespace pdft::module {

struct ModuleTraits {
    virtual void start(Module* module) const = 0;
    virtual Arc<Module> load(LoadContext& ctx, std::string name) const = 0;
};

template <typename T>
concept Startable = requires(T& value) {
    { value.start() } -> std::convertible_to<void>;
};

template <typename T>
ModuleTraits* getModuleTraits() {
    struct ModuleTraitsImpl : ModuleTraits {
        Arc<Module> load(LoadContext& ctx, std::string name) const override {
            return makeModule<T>(ctx, std::move(name));
        }

        void start(Module* module) const override {
            if constexpr (Startable<T>) {
                PDFT_VERIFY(dynamic_cast<T*>(module))->start();
            }
        }
    };

    static ModuleTraitsImpl traits;
    return &traits;
}

class FactoryStorage {
 public:
    using ModuleFactory = Arc<Module> (*)(LoadContext& ctx, std::string name);

    template <IsModule T>
    void registerModule(const std::string& factory) {
        PDFT_VERIFY(!factories_.contains(factory), "duplicate factory {}", factory);
        factories_.emplace(factory, getModuleTraits<T>());
    }

    ModuleTraits* getTraits(const std::string& factory) {
        PDFT_VERIFY(factories_.contains(factory), "unknown factory {}", factory);
        return factories_[factory];
    }

    static FactoryStorage& globalInstance() {
        static FactoryStorage storage;
        return storage;
    }

 private:
    FactoryStorage() = default;

    std::unordered_map<std::string, ModuleTraits*> factories_;
};

}  // namespace pdft::module
