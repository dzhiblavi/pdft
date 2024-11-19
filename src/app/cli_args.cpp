#include "cli_args.h"

#include <format>
#include <tclap/CmdLine.h>

namespace app {

std::expected<CliArgs, std::string> parseCliArgs(std::span<const char*> args) {
    try {
        CliArgs result;

        TCLAP::CmdLine cmd("pdft: A terminal PDF viewer", ' ', "0.0.1");
        TCLAP::ValueArg<std::string> config_param(
            "c", "config-file", "Path to a configuration file", false, "", "path", cmd);
        TCLAP::UnlabeledMultiArg<std::string> paths_param(
            "paths", "Paths to PDF files", true, "path", cmd);

        cmd.parse(args.size(), args.data());

        if (config_param.isSet()) {
            result.config_path = config_param.getValue();
        }

        result.paths = paths_param.getValue();
        return result;
    } catch (const TCLAP::ArgException& e) {
        return std::unexpected(std::format("Error for argument '{}': {}", e.argId(), e.error()));
    }
}

}  // namespace app
