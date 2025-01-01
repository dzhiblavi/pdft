#pragma once

#include "pdft/runtime/module/config.h"
#include "pdft/runtime/module/factory_storage.h"
#include "pdft/runtime/module/module.h"

#include "pdft/core/result.h"

#include <asio/io_context.hpp>
#include <rfl/type_name_t.hpp>

namespace pdft::rt {

using LoadedModules = std::unordered_map<std::string, Arc<Module>>;

class LoadContext {
 public:
    LoadContext(asio::io_context& io, ModulesConfig modules, FactoryStorage& factory_storage);

    Status loadAllModules();

    asio::io_context& getIoContext();
    std::optional<const ModuleConfig*> getConfig(const std::string& name);

    template <typename T>
    Result<Arc<T>> getModule(const std::string& name);
    Result<Arc<Module>> getModule(const std::string& name);

    template <typename T>
    Result<Arc<T>> loadModule(const std::string& name, const ModuleConfig& config);
    Result<Arc<Module>> loadModule(const std::string& name, const ModuleConfig& config);

 private:
    template <typename T>
    Result<Arc<T>> castModule(Arc<Module> module);

    bool isLoaded(const std::string& name);
    void addNewModule(const std::string& name, Arc<Module> module);
    Result<Arc<Module>> loadViaClass(const ModuleConfig& config, const std::string& name);
    Result<Arc<Module>> loadViaFactory(const ModuleConfig& config, const std::string& name);
    Result<Arc<Module>> loadViaAlias(const ModuleConfig& config, const std::string& name);

    asio::io_context& io_;
    FactoryStorage& factory_storage_;
    ModulesConfig modules_;
    LoadedModules loaded_ = {};
};

template <typename T>
Result<Arc<T>> LoadContext::getModule(const std::string& name) {
    co_return co_await castModule<T>(co_await getModule(name));
}

template <typename T>
Result<Arc<T>> LoadContext::loadModule(const std::string& name, const ModuleConfig& config) {
    co_return co_await castModule<T>(co_await loadModule(name, config));
}

template <typename T>
Result<Arc<T>> LoadContext::castModule(Arc<Module> module) {
    if (auto casted = std::dynamic_pointer_cast<T>(std::move(module))) {
        co_return std::move(casted);
    }

    co_return result::makeError(std::format(
        "type of module '{}' does not match the expected type '{}'",
        module->name(),
        rfl::type_name_t<T>().name()));
}

}  // namespace pdft::rt
