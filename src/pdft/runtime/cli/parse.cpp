#include "pdft/runtime/cli/parse.h"
#include "pdft/runtime/cli/storage.h"

#include "pdft/core/result.h"

#include <tclap/CmdLine.h>

namespace pdft::rt {

Result<bool> parseCliArgs(
    std::span<char const*> argv, const std::string& description, const std::string& version) {
    TCLAP::CmdLine cmd(description, ' ', version);
    cmd.setExceptionHandling(false);

    OptionsStorage::globalInstance().attach(cmd);

    try {
        cmd.parse(static_cast<int>(argv.size()), argv.data());
    } catch (const TCLAP::ArgException& e) {
        return result::makeError(std::format("Error for argument '{}': {}", e.argId(), e.error()));
    } catch (const TCLAP::ExitException& e) {
        return false;
    }

    return true;
}

}  // namespace pdft::rt
