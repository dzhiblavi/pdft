#pragma once

#include "pdft/core/module/loader.h"

#include <unordered_map>

namespace pdft {

struct ModuleTraits {
    using LoadVia = Arc<Module> (*)(ModuleLoader& loader, std::string name);
    using Start = void (*)(Module* module);

    LoadVia loadVia;
    Start start;
};

template <typename T>
concept Startable = requires(T& value) {
    { value.start() } -> std::convertible_to<void>;
};

template <Startable T>
ModuleTraits::Start makeStart() {
    return +[](Module* module) {
        auto ptr = dynamic_cast<T*>(module);
        ptr->start();
    };
}

template <typename T>
ModuleTraits::Start makeStart() {
    return +[](Module*) {};
}

template <typename T>
ModuleTraits makeModuleTraits() {
    ModuleTraits traits;

    traits.loadVia = +[](ModuleLoader& loader, std::string name) {
        return loader.template loadModule<T>(std::move(name));
    };

    traits.start = makeStart<T>();

    return traits;
}

class FactoryStorage {
 public:
    using ModuleFactory = Arc<Module> (*)(ModuleLoader& loader, std::string name);

    template <IsModule T>
    void registerModule(const std::string& module_name) {
        PDFT_VERIFY(!factories_.contains(module_name), "duplicate module {}", module_name);
        factories_.emplace(module_name, makeModuleTraits<T>());
    }

    ModuleTraits getTraits(const std::string& module_name) {
        PDFT_VERIFY(factories_.contains(module_name), "unknown module {}", module_name);
        return factories_[module_name];
    }

    static FactoryStorage& globalInstance() {
        static FactoryStorage storage;
        return storage;
    }

 private:
    FactoryStorage() = default;

    std::unordered_map<std::string, ModuleTraits> factories_;
};

}  // namespace pdft
