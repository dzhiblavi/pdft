#include "pdft/core/module/loader.h"
#include "pdft/core/module/factory_storage.h"

namespace pdft {

Arc<Module> ModuleLoader::loadModule(const std::string& name) {
    if (auto it = context_.loaded.find(name); it != context_.loaded.end()) {
        return it->second;
    }

    const auto& config = getConfig(name);
    auto factory = FactoryStorage::globalInstance().getTraits(config.factory).loadVia;
    return factory(*this, name);
}

const ModuleConfig& ModuleLoader::getConfig(const std::string& name) {
    PDFT_VERIFY(context_.modules.contains(name), "no config for module {}", name);
    return context_.modules[name];
}

ModulePtr ModuleLoader::addNewModule(const std::string& name, Arc<Module> module) {
    const auto& config = getConfig(name);
    module->setName(name);
    module->setLogger(PDFT_VERIFY(log::logger(config.logger)));
    context_.loaded[name] = module;
    log::info("Loaded a module: name={}", name);
    return module;
}

}  // namespace pdft
