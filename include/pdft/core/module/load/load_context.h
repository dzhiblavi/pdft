#pragma once

#include "pdft/core/module/config.h"
#include "pdft/core/module/module.h"

#include <asio/io_context.hpp>

namespace pdft::module {

using LoadedModules = std::unordered_map<std::string, Arc<Module>>;

struct LoadContext {
    asio::io_context& io;
    ModulesConfig modules;
    LoadedModules loaded = {};
};

}  // namespace pdft::module
