#pragma once

#include "app.h"

#include "pdft/app/state/palette_storage.h"

#include "pdft/core/module/module.h"

#include <span>

namespace app {

using pdft::inject;

class Entrypoint : public pdft::Module {
 public:
    Entrypoint(
        asio::io_context& io,
        std::span<const char*> argv,
        inject<pdft::app::PaletteStorage, "palettes"> palettes,
        inject<app::Application, "app"> app);

    void start();

 private:
    void printAppInfo() const;

    asio::io_context& io_;
    std::vector<std::string> paths_;
    pdft::Arc<app::Application> app_;
};

}  // namespace app
