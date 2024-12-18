#pragma once

#include <rfl/Generic.hpp>

#include <unordered_map>

namespace pdft::module {

struct ModuleConfig {
    // Either the module class name (i.e. pdft::app::Application)
    std::optional<std::string> cls = {};

    // Or the factory module name (i.e. apc_executor_factory, a module defined elsewhere)
    std::optional<std::string> factory = {};

    // Or an alias to another module
    std::optional<std::string> alias = {};

    rfl::Generic config = rfl::Generic::Null;
    std::string logger = "global";
};

using ModulesConfig = std::unordered_map<std::string, ModuleConfig>;

}  // namespace pdft::module
