#pragma once

#include "pdft/utils/cli/option.h"

#include "pdft/core/assert.h"

#include <tclap/CmdLine.h>

#include <map>

namespace pdft::cli {

class OptionsStorage {
 public:
    OptionsStorage() = default;

    void addOption(char sh, IOption* option) {
        PDFT_VERIFY(!options_.contains(sh), "duplicate short option -{}", sh);
        options_[sh] = option;
    }

    void attach(TCLAP::CmdLine& cmd) {
        for (auto& [_, option] : options_) {
            option->attach(cmd);
        }
    }

    static OptionsStorage& globalInstance() {
        static OptionsStorage storage;
        return storage;
    }

 private:
    std::map<char, IOption*> options_;
};

}  // namespace pdft::cli
