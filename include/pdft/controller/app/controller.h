#pragma once

#include "pdft/controller/controller.h"
#include "pdft/view/state.h"

namespace pdft::controller::app {

class Controller : public controller::Controller {
 public:
    Controller(
        std::shared_ptr<view::State> state, std::shared_ptr<media::pdf::DocumentStorage> docs)
        : state_(std::move(state))
        , docs_(std::move(docs)) {}

    Action onKeyEvent(char key) override;
    Action onFilesystemEvent(const notify::Event& event) override;

 private:
    std::shared_ptr<view::State> state_;
    std::shared_ptr<media::pdf::DocumentStorage> docs_;
};

}  // namespace pdft::controller::app
