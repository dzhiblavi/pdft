#pragma once

#include "pdft/core/module/traits.h"

#include <unordered_map>

namespace pdft::module {

class FactoryStorage {
 public:
    FactoryStorage() = default;

    void registerModule(const std::string& cls, ModuleTraits* traits);
    void registerFactoryModule(const std::string& cls, ModuleTraits* mod, FactoryTraits* fac);

    Result<ModuleTraits*> getModuleTraits(const std::string& cls);
    Result<FactoryTraits*> getFactoryTraits(const std::string& cls);

    static FactoryStorage& globalInstance();

 private:
    std::unordered_map<std::string, ModuleTraits*> modules_;
    std::unordered_map<std::string, FactoryTraits*> factories_;
};

}  // namespace pdft::module
