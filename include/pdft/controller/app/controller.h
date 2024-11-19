#pragma once

#include "pdft/controller/controller.h"
#include "pdft/controller/keytree.h"

#include "pdft/state/document_storage.h"
#include "pdft/state/state.h"

namespace pdft::controller::app {

class Controller : public controller::Controller {
 public:
    Controller(std::shared_ptr<state::State> state, std::shared_ptr<state::DocumentStorage> docs);

    Action onKeyEvent(char key) override;
    Action onFilesystemEvent(const notify::Event& event) override;

 private:
    KeyTree key_tree_;
    std::shared_ptr<state::State> state_;
    std::shared_ptr<state::DocumentStorage> docs_;
    Action action_ = Action::None;
};

}  // namespace pdft::controller::app
