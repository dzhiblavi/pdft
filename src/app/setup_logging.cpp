#include "setup_logging.h"

#include "pdft/core/log/config.h"
#include "pdft/core/log/load.h"
#include "pdft/core/log/log.h"

extern "C" {
#include <mupdf/fitz/context.h>
}

#include <filesystem>

namespace app {

using namespace pdft;

void setupLogging() {
    std::filesystem::path home = getenv("HOME");
    auto logs_dir = home / ".local/share/pdft";

    if (!std::filesystem::exists(logs_dir)) {
        std::filesystem::create_directories(logs_dir);
    }

    core::log::LogConfig log_config{
        .loggers =
            {
                {
                    "global",
                    {
                        .sinks =
                            {
                                core::log::SimpleFileSink{
                                    .path = home / ".local/share/pdft/app.log",
                                },
                            },
                    },
                },
                {
                    "mupdf",
                    {
                        .sinks =
                            {
                                core::log::SimpleFileSink{
                                    .path = home / ".local/share/pdft/mupdf.log",
                                },
                            },
                    },
                },
                {
                    "trace",
                    {
                        .level = spdlog::level::level_enum::trace,
                        .sinks =
                            {
                                core::log::SimpleFileSink{
                                    .path = home / ".local/share/pdft/trace.log",
                                },
                            },
                    },
                },
            },
    };

    core::log::loadConfig(log_config);
    core::log::global()->info("loggers have been successfully initialized");
}

void warningCallback(void* user, const char* message) {
    core::log::logger("mupdf")->warn(message);
}

void errorCallback(void* user, const char* message) {
    core::log::logger("mupdf")->warn(message);
}

void setupMupdfLogging(const pdft::media::pdf::Context& context) {
    fz_set_warning_callback(context.native(), warningCallback, nullptr);
    fz_set_error_callback(context.native(), errorCallback, nullptr);

    core::log::global()->info("mupdf loggers have been successfully initialized");
}

}  // namespace app
