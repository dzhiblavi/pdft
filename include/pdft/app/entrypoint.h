#pragma once

#include "pdft/app/app.h"

#include "platform/module/module.h"

namespace pdft::app {

class Entrypoint : public platform::rt::Module {
 public:
    Entrypoint(asio::io_context& io, platform::Arc<Application> app);

    void start() override;

 private:
    platform::Async<void> run();
    void printAppInfo() const;

    asio::io_context& io_;
    platform::Arc<Application> app_;
};

}  // namespace pdft::app
