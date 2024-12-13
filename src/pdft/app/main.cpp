#include "pdft/app/version.h"

#include "pdft/core/module/start.h"

int main(int argc, const char** argv) {
    auto description = "PDFT: A command line PDF reader";
    auto version = pdft::app::getFullVersionString();

    auto status = pdft::module::start(description, version, std::span(argv, argc));

    if (!status) {
        std::cerr << "error: " << status.error() << std::endl;
        return 1;
    }

    return *status;
}
