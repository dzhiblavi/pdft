#include "pdft/core/module/load/loader.h"
#include "pdft/core/module/detail/factory_storage.h"
#include "pdft/core/result/combine/lift.h"
#include "pdft/core/result/combine/unwrap.h"
#include "pdft/core/result/coro/result.h"  // IWYU pragma: keep
#include "pdft/core/result/make/none.h"
#include "pdft/core/result/syntax/pipe.h"

namespace pdft::module {

namespace {

bool isLoaded(LoadContext& ctx, const std::string& name) {
    return ctx.loaded.contains(name);
}

void addNewModule(LoadContext& ctx, const std::string& name, Arc<Module> module) {
    PDFT_VERIFY(!isLoaded(ctx, name), "duplicate loaded module {}", name);
    ctx.loaded[name] = module;
    log::info("successfully loaded a module: name={}", name);
}

}  // namespace

Status loadAllModules(LoadContext& ctx) {
    for (const auto& [name, conf] : ctx.modules) {
        co_await module::getOrLoadModule(ctx, name);
    }

    co_return unit;
}

Maybe<const ModuleConfig*> getConfig(LoadContext& ctx, const std::string& name) {
    if (!ctx.modules.contains(name)) {
        return result::none;
    }

    return &ctx.modules[name];
}

Result<Arc<Module>> getOrLoadModule(LoadContext& ctx, const std::string& name) {
    if (auto it = ctx.loaded.find(name); it != ctx.loaded.end()) {
        co_return it->second;
    }

    const auto& config =
        co_await (getConfig(ctx, name) | result::lift("no config for module '{}'", name));
    Arc<Module> instance;

    if (config->cls.has_value()) {
        auto&& cls = *config->cls;
        log::info("loading a module '{}' via class implementation '{}'", name, cls);
        auto* traits = co_await detail::FactoryStorage::globalInstance().getModuleTraits(cls);
        instance = co_await traits->load(ctx, name);
        traits->start(instance.get());
    } else if (config->factory.has_value()) {
        auto&& fac = *config->factory;
        log::info("loading a module '{}' via factory '{}'", name, fac);

        auto factory = co_await getOrLoadModule(ctx, *config->factory);
        const auto& factory_config = getConfig(ctx, fac) | result::unwrap;
        PDFT_VERIFY(factory_config->cls, "factory factories are not supported");

        auto* traits = co_await detail::FactoryStorage::globalInstance().getFactoryTraits(
            *factory_config->cls);
        instance = co_await traits->create(factory.get());
    } else {
        co_return std::format(
            "neither factory nor cls construction methods are provided for module '{}'", name);
    }

    addNewModule(ctx, name, instance);
    co_return instance;
}

}  // namespace pdft::module
