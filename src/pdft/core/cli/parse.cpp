#include "pdft/core/cli/parse.h"

#include "pdft/core/result/make/err.h"
#include "pdft/core/result/make/ok.h"

#include "pdft/core/cli/storage.h"

#include <tclap/CmdLine.h>

namespace pdft::cli {

Result<bool> parse(
    std::span<char const*> argv, const std::string& description, const std::string& version) {
    TCLAP::CmdLine cmd(description, ' ', version);
    cmd.setExceptionHandling(false);

    cli::OptionsStorage::globalInstance().attach(cmd);

    try {
        cmd.parse(static_cast<int>(argv.size()), argv.data());
    } catch (const TCLAP::ArgException& e) {
        return result::err(std::format("Error for argument '{}': {}", e.argId(), e.error()));
    } catch (const TCLAP::ExitException& e) {
        return result::ok(false);
    }

    return result::ok(true);
}

}  // namespace pdft::cli
