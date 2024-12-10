#pragma once

#include <rfl/Generic.hpp>

#include <unordered_map>

namespace pdft::module {

struct ModuleConfig {
    std::string factory;
    rfl::Generic config = rfl::Generic::Null;
    std::string logger = "global";
};

using ModulesConfig = std::unordered_map<std::string, ModuleConfig>;

}  // namespace pdft::module
