#include "logging.h"

#include "pdft/core/log/config.h"
#include "pdft/core/log/load.h"
#include "pdft/core/log/log.h"

extern "C" {
#include <mupdf/fitz/context.h>
}

#include <filesystem>

namespace app {

using namespace pdft;

namespace {

spdlog::level::level_enum toSpdlogLevel(LogLevel level) {
    return static_cast<spdlog::level::level_enum>(level);
}

}  // namespace

void setupLogging(const LoggingConfig& config) {
    auto directory = utils::expandHome(config.directory);

    if (!std::filesystem::exists(directory)) {
        std::filesystem::create_directories(directory);
    }

    core::log::LogConfig log_config{
        .loggers =
            {
                {
                    "global",
                    {
                        .level = toSpdlogLevel(config.level),
                        .format = "%Y-%m-%dT%T %z %^%s:%#%$ %L %v",
                        .sinks =
                            {
                                core::log::SimpleFileSink{
                                    .path = directory / "app.log",
                                },
                            },
                    },
                },
                {
                    "mupdf",
                    {
                        .level = toSpdlogLevel(config.level),
                        .sinks =
                            {
                                core::log::SimpleFileSink{
                                    .path = directory / "mupdf.log",
                                },
                            },
                    },
                },
                {
                    "trace",
                    {
                        // Trace logger always has the trace logging level
                        .level = spdlog::level::level_enum::trace,
                        .sinks =
                            {
                                core::log::SimpleFileSink{
                                    .path = directory / "trace.log",
                                },
                            },
                    },
                },
            },
    };

    core::log::loadConfig(log_config);
}

void warningCallback(void*, const char* message) {
    core::log::logger("mupdf")->warn(message);
}

void errorCallback(void*, const char* message) {
    core::log::logger("mupdf")->warn(message);
}

void setupMupdfLogging(const pdft::media::mupdf::Context& context) {
    fz_set_warning_callback(context.native(), warningCallback, nullptr);
    fz_set_error_callback(context.native(), errorCallback, nullptr);

    log::debug("mupdf loggers have been successfully initialized");
}

}  // namespace app
