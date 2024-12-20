#pragma once

#include "pdft/runtime/cli/option.h"

#include <map>

namespace pdft::rt {

class OptionsStorage {
 public:
    OptionsStorage() = default;

    void addOption(char sh, IOption* option);
    void attach(TCLAP::CmdLine& cmd);

    static OptionsStorage& globalInstance();

 private:
    std::map<char, IOption*> options_;
};

}  // namespace pdft::rt
