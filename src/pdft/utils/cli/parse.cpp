#include "pdft/utils/cli/parse.h"

#include "pdft/core/result/make/err.h"
#include "pdft/core/result/make/ok.h"

#include "pdft/utils/cli/detail/storage.h"

#include <tclap/CmdLine.h>

namespace pdft::cli {

Status parse(
    std::span<char const*> argv, const std::string& description, const std::string& version) {
    TCLAP::CmdLine cmd(description, ' ', version);
    cmd.setExceptionHandling(false);

    cli::detail::OptionsStorage::globalInstance().attach(cmd);

    try {
        cmd.parse(argv.size(), argv.data());
    } catch (const TCLAP::ArgException& e) {
        return result::err(std::format("Error for argument '{}': {}", e.argId(), e.error()));
    } catch (const TCLAP::ExitException& e) {
        return result::err("");
    }

    return result::ok();
}

}  // namespace pdft::cli
