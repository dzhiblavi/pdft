#pragma once

#include "pdft/controller/controller.h"
#include "pdft/controller/keytree.h"

#include "pdft/state/document_storage.h"
#include "pdft/state/state.h"

#include <vector>

namespace pdft::controller::app {

class Controller : public controller::Controller {
 public:
    enum class Command {
        Quit,
        ScrollNext,
        ScrollPrev,
        NumNextPage,
        NumPrevPage,
        GoToFirstPage,
        GoToLastPage,
        NumGoToPage,
        NextDocument,
        PrevDocument,
        NumNextDocument,
        NumPrevDocument,
        NextLayout,
        PrevLayout,
        ZoomIn,
        ZoomOut,
    };

    struct KeyMapping {
        Command command;
        std::string keys;
    };

    struct Config {
        std::vector<KeyMapping> key_mappings;
    };

    Controller(
        const Config& config,
        std::shared_ptr<state::State> state,
        std::shared_ptr<state::DocumentStorage> docs);

    Action onKeyEvent(char key) override;
    Action onFilesystemEvent(const notify::Event& event) override;

 private:
    void bindKey(Command command, std::string_view keys);

    KeyTree key_tree_;
    std::shared_ptr<state::State> state_;
    std::shared_ptr<state::DocumentStorage> docs_;
    bool quit_ = false;
};

}  // namespace pdft::controller::app
