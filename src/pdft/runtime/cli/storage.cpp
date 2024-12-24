#include "pdft/runtime/cli/storage.h"

#include "pdft/core/assert.h"

namespace pdft::rt {

void OptionsStorage::addOption(char sh, IOption* option) {
    verify(!options_.contains(sh), "duplicate short option -{}", sh);
    options_[sh] = option;
}

void OptionsStorage::attach(TCLAP::CmdLine& cmd) {
    for (auto& [_, option] : options_) {
        option->attach(cmd);
    }
}

OptionsStorage& OptionsStorage::globalInstance() {
    static OptionsStorage storage;
    return storage;
}

}  // namespace pdft::rt
