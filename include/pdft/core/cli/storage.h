#pragma once

#include "pdft/core/cli/option.h"

#include <tclap/CmdLine.h>

#include <map>

namespace pdft::cli {

class OptionsStorage {
 public:
    OptionsStorage() = default;

    void addOption(char sh, IOption* option);
    void attach(TCLAP::CmdLine& cmd);

    static OptionsStorage& globalInstance();

 private:
    std::map<char, IOption*> options_;
};

}  // namespace pdft::cli
