#pragma once

#include "pdft/core/module/load/load_context.h"
#include "pdft/core/result/type/status.h"

namespace pdft::module {

Status loadAllModules(LoadContext& ctx);

const ModuleConfig& getConfig(LoadContext& ctx, const std::string& name);

Result<Arc<Module>> getOrLoadModule(LoadContext& ctx, const std::string& name);

}  // namespace pdft::module
