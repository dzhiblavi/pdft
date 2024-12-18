#include "pdft/core/module/load.h"

#include "pdft/core/result/combine/lift.h"
#include "pdft/core/result/combine/map_err.h"
#include "pdft/core/result/combine/unwrap.h"
#include "pdft/core/result/coro/result.h"  // IWYU pragma: keep
#include "pdft/core/result/make/none.h"
#include "pdft/core/result/syntax/pipe.h"

namespace pdft::module {

using namespace result;

LoadContext::LoadContext(
    asio::io_context& io, ModulesConfig modules, FactoryStorage& factory_storage)
    : io_(io)
    , factory_storage_(factory_storage)
    , modules_(std::move(modules)) {}

asio::io_context& LoadContext::getIoContext() {
    return io_;
}

bool LoadContext::isLoaded(const std::string& name) {
    return loaded_.contains(name);
}

void LoadContext::addNewModule(const std::string& name, Arc<Module> module) {
    PDFT_VERIFY(!isLoaded(name), "duplicate loaded module {}", name);
    loaded_[name] = module;
    log::info("successfully loaded a module: name={}", name);
}

Status LoadContext::loadAllModules() {
    for (const auto& [name, conf] : modules_) {
        co_await getModule(name);
    }

    co_return unit;
}

Maybe<const ModuleConfig*> LoadContext::getConfig(const std::string& name) {
    auto it = modules_.find(name);
    if (it == modules_.end()) {
        return none;
    }
    return &it->second;
}

Result<Arc<Module>> LoadContext::load(const ModuleConfig& config, const std::string& name) {
    if (config.cls.has_value()) {
        return loadViaClass(config, name);
    } else if (config.factory.has_value()) {
        return loadViaFactory(config, name);
    } else if (config.alias.has_value()) {
        return loadViaAlias(config, name);
    } else {
        return err(
            "neither factory nor cls construction methods are provided for module '{}'", name);
    }
}

Result<Arc<Module>> LoadContext::loadViaClass(const ModuleConfig& config, const std::string& name) {
    auto&& cls = *config.cls;
    log::info("loading a module '{}' via class implementation '{}'", name, cls);

    // Retrieve module traits for the requested module class
    auto* traits = co_await factory_storage_.getModuleTraits(cls);

    // Try to create a module
    auto instance = co_await traits->load(*this, name);

    // Start the module
    traits->start(instance.get());

    co_return instance;
}

Result<Arc<Module>> LoadContext::loadViaFactory(
    const ModuleConfig& config, const std::string& name) {
    auto&& fac = *config.factory;
    log::info("loading a module '{}' via factory '{}'", name, fac);

    // Try to get the factory module
    auto factory = co_await getModule(fac);

    // Check that factory is not created via factory (not supported)
    const auto& factory_config = getConfig(fac) | unwrap;
    if (!factory_config->cls) {
        co_return err("factory factories are not supported");
    }

    // Get the factory class traits (unwrap because we have already created the module successfully)
    auto* traits = factory_storage_.getFactoryTraits(*factory_config->cls) | unwrap;

    // Use factory to create a requested module
    co_return co_await traits->create(factory.get());
}

Result<Arc<Module>> LoadContext::loadViaAlias(const ModuleConfig& config, const std::string& name) {
    auto&& alias = *config.alias;
    log::info("loading a module '{}' as alias to '{}'", name, alias);
    co_return co_await getModule(alias);
}

Result<Arc<Module>> LoadContext::getModule(const std::string& name) {
    if (auto it = loaded_.find(name); it != loaded_.end()) {
        co_return it->second;
    }

    const auto& config = co_await (getConfig(name) | lift("no config for module '{}'", name));

    auto map_err = [&name](const auto& err) {
        return std::format("while loading module '{}':\n{}", name, err);
    };

    auto instance = co_await (load(*config, name) | mapErr(map_err));
    addNewModule(name, instance);
    co_return instance;
}

}  // namespace pdft::module
