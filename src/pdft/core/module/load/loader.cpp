#include "pdft/core/module/load/loader.h"

#include "pdft/core/module/factory.h"
#include "pdft/core/module/factory_storage.h"

namespace pdft::module {

void loadAllModules(LoadContext& ctx) {
    for (const auto& [name, conf] : ctx.modules) {
        module::getOrLoadModule(ctx, name);
    }
}

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
    Arc<Module> instance;

    if (config.cls.has_value()) {
        log::info("loading a module '{}' via class implementation '{}'", name, *config.cls);
        auto* traits = FactoryStorage::globalInstance().getModuleTraits(*config.cls);
        instance = traits->load(ctx, name);
        traits->start(instance.get());
    } else if (config.factory.has_value()) {
        log::info("loading a module '{}' via factory '{}'", name, *config.factory);
        PDFT_VERIFY(
            config.factory.has_value(),
            "neither 'cls' nor 'factory' fields have not been provided for module '{}'",
            name);

        const auto& factory_config = getConfig(ctx, *config.factory);
        PDFT_VERIFY(factory_config.cls, "factory factories are not supported");

        auto factory = getOrLoadModule(ctx, *config.factory);
        auto* traits = FactoryStorage::globalInstance().getFactoryTraits(*factory_config.cls);
        instance = traits->create(factory.get());
    } else {
        panic("neither factory nor cls construction methods are provided for module '{}'", name);
    }

    return addNewModule(ctx, name, instance);
}

bool isLoaded(LoadContext& ctx, const std::string& name) {
    return ctx.loaded.contains(name);
}

}  // namespace pdft::module
