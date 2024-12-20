#pragma once

#include "pdft/runtime/module/config.h"
#include "pdft/runtime/module/module.h"

#include "pdft/core/result/type/result.h"

#include <rfl/Generic.hpp>

namespace pdft::rt {

class LoadContext;

struct ModuleTraits {
    virtual ~ModuleTraits() = default;

    virtual Result<Arc<Module>> createVia(
        LoadContext& ctx, const ModuleConfig& config, const std::string& name) const = 0;

    virtual rfl::Generic getConfig() const = 0;
};

struct FactoryTraits {
    virtual ~FactoryTraits() = default;
    virtual Result<Arc<Module>> create(Module* self) const = 0;
};

}  // namespace pdft::rt
