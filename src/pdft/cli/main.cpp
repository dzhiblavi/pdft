#include "pdft/app/version.h"

#include "pdft/runtime/start.h"

#include <iostream>

int main(int argc, const char** argv) {
    auto description = "PDFT: A command line PDF reader";
    auto version = pdft::app::getFullVersionString();

    auto status = pdft::rt::start(description, version, std::span(argv, argc));

    if (!status) {
        std::cerr << "error: " << status.error<std::string>() << std::endl;
        return 1;
    }

    return *status;
}
