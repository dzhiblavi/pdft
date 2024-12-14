#pragma once

#include "pdft/core/module/traits.h"
#include "pdft/core/result/type/status.h"

#include <unordered_map>

namespace pdft::module {

class FactoryStorage {
 public:
    FactoryStorage() = default;

    Status registerModule(const std::string& cls, ModuleTraits* traits);
    Status registerFactoryModule(const std::string& cls, ModuleTraits* mod, FactoryTraits* fac);

    Result<ModuleTraits*> getModuleTraits(const std::string& cls);
    Result<FactoryTraits*> getFactoryTraits(const std::string& cls);

    static FactoryStorage& globalInstance();

 private:
    std::unordered_map<std::string, ModuleTraits*> modules_;
    std::unordered_map<std::string, FactoryTraits*> factories_;
};

}  // namespace pdft::module
