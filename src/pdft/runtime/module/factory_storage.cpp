#include "pdft/runtime/module/factory_storage.h"

#include "pdft/core/result.h"

namespace pdft::rt {

Status FactoryStorage::registerModule(const std::string& cls, ModuleTraits* traits) {
    if (modules_.contains(cls)) {
        return result::makeError(std::format("duplicate module {}", cls));
    }

    modules_.emplace(cls, traits);
    return result::unit;
}

Status FactoryStorage::registerFactoryModule(
    const std::string& cls, ModuleTraits* mod, FactoryTraits* fac) {
    co_await registerModule(cls, mod);

    if (factories_.contains(cls)) {
        co_return result::makeError(std::format("duplicate factory {}", cls));
    }

    factories_.emplace(cls, fac);
    co_return result::unit;
}

Result<ModuleTraits*> FactoryStorage::getModuleTraits(const std::string& cls) {
    if (!modules_.contains(cls)) {
        return result::makeError(std::format("unknown module {}", cls));
    }

    return modules_[cls];
}

Result<FactoryTraits*> FactoryStorage::getFactoryTraits(const std::string& cls) {
    if (!factories_.contains(cls)) {
        return result::makeError(std::format("unknown factory {}", cls));
    }

    return factories_[cls];
}

FactoryStorage& FactoryStorage::globalInstance() {
    static FactoryStorage storage;
    return storage;
}

const std::unordered_map<std::string, ModuleTraits*>& FactoryStorage::getModules() const {
    return modules_;
}

}  // namespace pdft::rt
