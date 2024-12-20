#pragma once

#include "pdft/app/app.h"

#include "pdft/runtime/module/module.h"

namespace pdft::app {

class Entrypoint : public rt::Module {
 public:
    Entrypoint(asio::io_context& io, Arc<Application> app);

    void start() override;

 private:
    Async<void> run();
    void printAppInfo() const;

    asio::io_context& io_;
    Arc<Application> app_;
};

}  // namespace pdft::app
