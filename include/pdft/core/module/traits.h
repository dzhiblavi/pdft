#pragma once

#include "pdft/core/module/module.h"
#include "pdft/core/result/type/result.h"

#include <rfl/Generic.hpp>

namespace pdft::module {

class LoadContext;

struct ModuleTraits {
    virtual ~ModuleTraits() = default;
    virtual void start(Module* self) const = 0;
    virtual Result<Arc<Module>> load(LoadContext& ctx, std::string name) const = 0;
    virtual rfl::Generic getConfig() const = 0;
};

struct FactoryTraits {
    virtual ~FactoryTraits() = default;
    virtual Result<Arc<Module>> create(Module* self) const = 0;
};

}  // namespace pdft::module
