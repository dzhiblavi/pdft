#pragma once

#include "pdft/core/module/load/load_context.h"

namespace pdft::module {

void loadAllModules(LoadContext& ctx);

const ModuleConfig& getConfig(LoadContext& ctx, const std::string& name);

Arc<Module> addNewModule(LoadContext& ctx, const std::string& name, Arc<Module> module);

bool isLoaded(LoadContext& ctx, const std::string& name);

Arc<Module> getOrLoadModule(LoadContext& ctx, const std::string& name);

}  // namespace pdft::module
