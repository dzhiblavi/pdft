#pragma once

#include "pdft/core/module/load/load_context.h"
#include "pdft/core/result/type/result.h"

#include <unordered_map>

namespace pdft::module::detail {

struct ModuleTraits {
    virtual void start(Module* self) const = 0;
    virtual Arc<Module> load(LoadContext& ctx, std::string name) const = 0;
};

struct FactoryTraits {
    virtual Arc<Module> create(Module* self) const = 0;
};

class FactoryStorage {
 public:
    using ModuleFactory = Arc<Module> (*)(LoadContext& ctx, std::string name);

    void registerModule(const std::string& cls, ModuleTraits* traits);
    void registerFactoryModule(const std::string& cls, ModuleTraits* mod, FactoryTraits* fac);

    Result<ModuleTraits*> getModuleTraits(const std::string& cls);
    Result<FactoryTraits*> getFactoryTraits(const std::string& cls);

    static FactoryStorage& globalInstance();

 private:
    FactoryStorage() = default;

    std::unordered_map<std::string, ModuleTraits*> modules_;
    std::unordered_map<std::string, FactoryTraits*> factories_;
};

}  // namespace pdft::module::detail
