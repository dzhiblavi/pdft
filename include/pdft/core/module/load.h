#pragma once

#include "pdft/core/module/config.h"
#include "pdft/core/module/factory_storage.h"
#include "pdft/core/module/module.h"

#include "pdft/core/result/coro/result.h"  // IWYU pragma: keep
#include "pdft/core/result/make/err.h"
#include "pdft/core/result/type/maybe.h"
#include "pdft/core/result/type/status.h"

#include <asio/io_context.hpp>

namespace pdft::module {

using LoadedModules = std::unordered_map<std::string, Arc<Module>>;

class LoadContext {
 public:
    LoadContext(asio::io_context& io, ModulesConfig modules, FactoryStorage& factory_storage);

    template <typename T>
    Result<Arc<T>> getModule(const std::string& name);

    Result<Arc<Module>> getModule(const std::string& name);
    Status loadAllModules();

    asio::io_context& getIoContext();
    Maybe<const ModuleConfig*> getConfig(const std::string& name);

 private:
    bool isLoaded(const std::string& name);
    void addNewModule(const std::string& name, Arc<Module> module);
    Result<Arc<Module>> load(const ModuleConfig& config, const std::string& name);
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
    auto raw = co_await getModule(name);

    if (auto casted = std::dynamic_pointer_cast<T>(std::move(raw))) {
        co_return std::move(casted);
    }

    co_return result::err(
        "type of loaded module '{}' does not match the expected type '{}'",
        name,
        rfl::type_name_t<T>().name());
}

}  // namespace pdft::module
