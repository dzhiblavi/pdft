#include "pdft/core/module/load/loader.h"
#include "pdft/core/module/factory_storage.h"

namespace pdft::module {

const ModuleConfig& getConfig(LoadContext& ctx, const std::string& name) {
    PDFT_VERIFY(ctx.modules.contains(name), "no config for module {}", name);
    return ctx.modules[name];
}

Arc<Module> addNewModule(LoadContext& ctx, const std::string& name, Arc<Module> module) {
    PDFT_VERIFY(!isLoaded(ctx, name), "duplicate loaded module {}", name);
    ctx.loaded[name] = module;
    log::info("successfully loaded a module: name={}", name);
    return module;
}

Arc<Module> getOrLoadModule(LoadContext& ctx, const std::string& name) {
    if (auto it = ctx.loaded.find(name); it != ctx.loaded.end()) {
        return it->second;
    }

    const auto& config = getConfig(ctx, name);
    auto* traits = FactoryStorage::globalInstance().getTraits(config.factory);

    return addNewModule(ctx, name, traits->load(ctx, name));
}

bool isLoaded(LoadContext& ctx, const std::string& name) {
    return ctx.loaded.contains(name);
}

}  // namespace pdft::module
