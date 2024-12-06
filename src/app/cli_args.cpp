#include "cli_args.h"

#include "version.h"

#include <magic_enum.hpp>
#include <tclap/CmdLine.h>

#include <format>

namespace app {

std::expected<CliArgs, std::string> parseCliArgs(std::span<const char*> args) {
    try {
        CliArgs result;

        TCLAP::CmdLine cmd("pdft: A terminal PDF viewer", ' ', getFullVersionString());
        TCLAP::ValueArg<std::string> config_param(
            "c", "config-file", "Path to a configuration file", false, "", "path", cmd);
        TCLAP::UnlabeledMultiArg<std::string> paths_param(
            "paths", "Paths to PDF files", false, "path", cmd);

        TCLAP::ValueArg<std::string> state_mode_param(
            "s", "state-mode", "State operation mode", false, "", "enum", cmd);

        cmd.parse(args.size(), args.data());

        if (config_param.isSet()) {
            result.config_path = config_param.getValue();
        }

        auto&& state_mode_str = state_mode_param.getValue();

        if (!state_mode_str.empty()) {
            auto state_mode = magic_enum::enum_cast<pdft::app::StateMode>(state_mode_str);

            if (!state_mode) {
                return std::unexpected(std::format("Invalid state mode: '{}'", state_mode_str));
            }

            result.state_mode = *state_mode;
        }

        result.paths = paths_param.getValue();
        return result;
    } catch (const TCLAP::ArgException& e) {
        return std::unexpected(std::format("Error for argument '{}': {}", e.argId(), e.error()));
    }
}

}  // namespace app
