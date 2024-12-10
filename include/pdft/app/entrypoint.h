#pragma once

#include "pdft/app/app.h"
#include "pdft/app/state/palette_storage.h"

#include "pdft/core/module/module.h"

#include <span>

namespace pdft::app {

class Entrypoint : public Module {
 public:
    Entrypoint(
        asio::io_context& io,
        std::span<const char*> argv,
        inject<PaletteStorage, "palettes"> palettes,
        inject<Application, "app"> app);

    void start();

 private:
    void printAppInfo() const;

    asio::io_context& io_;
    std::vector<std::string> paths_;
    Arc<Application> app_;
};

}  // namespace pdft::app
