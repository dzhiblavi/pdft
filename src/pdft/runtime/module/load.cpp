#include "pdft/runtime/module/load.h"

#include "pdft/core/result.h"

namespace pdft::rt {

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
    verify(!isLoaded(name), "duplicate loaded module {}", name);
    loaded_[name] = module;
    log::info("successfully loaded a module: name={}", name);
}

Status LoadContext::loadAllModules() {
    for (const auto& [name, conf] : modules_) {
        co_await getModule(name);
    }

    co_return unit;
}

std::optional<const ModuleConfig*> LoadContext::getConfig(const std::string& name) {
    auto it = modules_.find(name);
    if (it == modules_.end()) {
        return std::nullopt;
    }
    return &it->second;
}

Result<Arc<Module>> LoadContext::loadModule(const std::string& name, const ModuleConfig& config) {
    Arc<Module> instance;

    if (config.cls.has_value()) {
        instance = co_await loadViaClass(config, name);
    } else if (config.factory.has_value()) {
        instance = co_await loadViaFactory(config, name);
    } else if (config.alias.has_value()) {
        instance = co_await loadViaAlias(config, name);
    } else {
        co_return result::makeError(std::format(
            "neither factory nor cls construction methods are provided for module '{}'", name));
    }

    log::LoggerPtr logger;
    if (config.logger != "global") {
        logger = log::logger(config.logger);
    } else {
        logger = log::global();
    }

    if (!logger) {
        co_return result::makeError(
            std::format("missing logger {} required by module '{}'", config.logger, name));
    }

    instance->setLogger(logger);
    instance->setName(std::move(name));
    instance->start();

    co_return instance;
}

Result<Arc<Module>> LoadContext::loadViaClass(const ModuleConfig& config, const std::string& name) {
    auto&& cls = *config.cls;
    log::info("loading a module '{}' via class implementation '{}'", name, cls);

    auto* traits = co_await factory_storage_.getModuleTraits(cls);
    co_return co_await traits->createVia(*this, config, name);
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
        co_return result::makeError<std::string>("factory factories and aliases are not supported");
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

    const auto& config =
        co_await (getConfig(name) | lift(std::format("no config for module '{}'", name)));
    auto instance = co_await loadModule(name, *config);
    addNewModule(name, instance);
    co_return instance;
}

}  // namespace pdft::rt
