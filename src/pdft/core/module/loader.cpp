#include "pdft/core/module/loader.h"
#include "pdft/core/module/factory_storage.h"

namespace pdft::module {

Arc<Module> ModuleLoader::getOrLoadModule(const std::string& name) {
    if (auto it = context_.loaded.find(name); it != context_.loaded.end()) {
        return it->second;
    }

    const auto& config = getConfig(name);
    auto* traits = FactoryStorage::globalInstance().getTraits(config.factory);

    return addNewModule(name, traits->load(*this, name));
}

const ModuleConfig& ModuleLoader::getConfig(const std::string& name) {
    PDFT_VERIFY(context_.modules.contains(name), "no config for module {}", name);
    return context_.modules[name];
}

ModulePtr ModuleLoader::addNewModule(const std::string& name, Arc<Module> module) {
    PDFT_VERIFY(!isLoaded(name), "duplicate loaded module {}", name);
    context_.loaded[name] = module;
    log::info("successfully loaded a module: name={}", name);
    return module;
}

bool ModuleLoader::isLoaded(const std::string& name) {
    return context_.loaded.contains(name);
}

}  // namespace pdft::module
