#include "pdft/core/module/detail/factory_storage.h"
#include "pdft/core/result/make/err.h"

namespace pdft::module::detail {

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

Result<ModuleTraits*> FactoryStorage::getModuleTraits(const std::string& cls) {
    if (!modules_.contains(cls)) {
        return result::err(std::format("unknown module {}", cls));
    }

    return modules_[cls];
}

Result<FactoryTraits*> FactoryStorage::getFactoryTraits(const std::string& cls) {
    if (!factories_.contains(cls)) {
        return result::err(std::format("unknown factory {}", cls));
    }

    return factories_[cls];
}

FactoryStorage& FactoryStorage::globalInstance() {
    static FactoryStorage storage;
    return storage;
}

}  // namespace pdft::module::detail
