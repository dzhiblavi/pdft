#pragma once

#include "pdft/core/module/load/detail/constructor.h"
#include "pdft/core/module/load/detail/inject.h"
#include "pdft/core/module/load/detail/injector.h"
#include "pdft/core/module/load/load_context.h"

namespace pdft::module {

void loadAllModules(LoadContext& ctx);

const ModuleConfig& getConfig(LoadContext& ctx, const std::string& name);

Arc<Module> addNewModule(LoadContext& ctx, const std::string& name, Arc<Module> module);

bool isLoaded(LoadContext& ctx, const std::string& name);

Arc<Module> getOrLoadModule(LoadContext& ctx, const std::string& name);

template <IsModule T>
Arc<T> makeModule(LoadContext& ctx, std::string name) {
    using Parameters = typename detail::Constructor<T>::type;

    Arc<T> module = std::apply(
        [&, name]<typename... Types>(Types...) {
            return std::make_shared<T>(std::conditional_t<
                                       Types::value,
                                       detail::RefArgumentInjector<T>,
                                       detail::ValueArgumentInjector<T>>(ctx, name)...);
        },
        Parameters{});

    const auto& config = getConfig(ctx, name);

    module->setLogger(PDFT_VERIFY(
        log::logger(config.logger),
        "missing logger {} required by module {}",
        config.logger,
        name));

    module->setName(std::move(name));
    return module;
}

}  // namespace pdft::module
