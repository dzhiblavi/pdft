#include "pdft/core/module/factory_storage.h"

namespace pdft::module {

void FactoryStorage::registerModule(const std::string& cls, ModuleTraits* traits) {
    PDFT_VERIFY(!modules_.contains(cls), "duplicate module {}", cls);
    modules_.emplace(cls, traits);
}

void FactoryStorage::registerFactoryModule(
    const std::string& cls, ModuleTraits* mod, FactoryTraits* fac) {
    registerModule(cls, mod);
    PDFT_VERIFY(!factories_.contains(cls), "duplicate factory {}", cls);
    factories_.emplace(cls, fac);
}

ModuleTraits* FactoryStorage::getModuleTraits(const std::string& cls) {
    PDFT_VERIFY(modules_.contains(cls), "unknown module {}", cls);
    return modules_[cls];
}

FactoryTraits* FactoryStorage::getFactoryTraits(const std::string& cls) {
    PDFT_VERIFY(factories_.contains(cls), "unknown factory {}", cls);
    return factories_[cls];
}

FactoryStorage& FactoryStorage::globalInstance() {
    static FactoryStorage storage;
    return storage;
}

}  // namespace pdft::module
