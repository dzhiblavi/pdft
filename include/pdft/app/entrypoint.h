#pragma once

#include "pdft/app/app.h"

#include "pdft/core/module/module.h"

namespace pdft::app {

class Entrypoint : public Module {
 public:
    Entrypoint(asio::io_context& io, inject<Application, "app"> app);

    void start();

 private:
    void printAppInfo() const;

    asio::io_context& io_;
    Arc<Application> app_;
};

}  // namespace pdft::app
