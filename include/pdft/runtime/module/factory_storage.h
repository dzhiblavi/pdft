#pragma once

#include "pdft/runtime/module/traits.h"

#include "pdft/core/result/type/status.h"

#include <unordered_map>

namespace pdft::rt {

class FactoryStorage {
 public:
    FactoryStorage() = default;

    Status registerModule(const std::string& cls, ModuleTraits* traits);
    Status registerFactoryModule(const std::string& cls, ModuleTraits* mod, FactoryTraits* fac);

    Result<ModuleTraits*> getModuleTraits(const std::string& cls);
    Result<FactoryTraits*> getFactoryTraits(const std::string& cls);

    const std::unordered_map<std::string, ModuleTraits*>& getModules() const;

    static FactoryStorage& globalInstance();

 private:
    std::unordered_map<std::string, ModuleTraits*> modules_;
    std::unordered_map<std::string, FactoryTraits*> factories_;
};

}  // namespace pdft::rt
