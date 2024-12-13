#include "pdft/core/module/load.h"
#include "pdft/core/module/detail/factory_storage.h"

#include "pdft/core/result/combine/lift.h"
#include "pdft/core/result/combine/unwrap.h"
#include "pdft/core/result/coro/result.h"  // IWYU pragma: keep
#include "pdft/core/result/make/none.h"
#include "pdft/core/result/syntax/pipe.h"

namespace pdft::module {

LoadContext::LoadContext(asio::io_context& io, ModulesConfig modules)
    : io_(io)
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
        return result::none;
    }
    return &it->second;
}

Result<Arc<Module>> LoadContext::getModule(const std::string& name) {
    if (auto it = loaded_.find(name); it != loaded_.end()) {
        co_return it->second;
    }

    const auto& config =
        co_await (getConfig(name) | result::lift("no config for module '{}'", name));
    Arc<Module> instance;

    if (config->cls.has_value()) {
        auto&& cls = *config->cls;
        log::info("loading a module '{}' via class implementation '{}'", name, cls);
        auto* traits = co_await detail::FactoryStorage::globalInstance().getModuleTraits(cls);
        instance = co_await traits->load(*this, name);
        traits->start(instance.get());
    } else if (config->factory.has_value()) {
        auto&& fac = *config->factory;
        log::info("loading a module '{}' via factory '{}'", name, fac);

        auto factory = co_await getModule(*config->factory);
        const auto& factory_config = getConfig(fac) | result::unwrap;
        PDFT_VERIFY(factory_config->cls, "factory factories are not supported");

        auto* traits = co_await detail::FactoryStorage::globalInstance().getFactoryTraits(
            *factory_config->cls);
        instance = co_await traits->create(factory.get());
    } else {
        co_return std::format(
            "neither factory nor cls construction methods are provided for module '{}'", name);
    }

    addNewModule(name, instance);
    co_return instance;
}

}  // namespace pdft::module
