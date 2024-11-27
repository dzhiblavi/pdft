#pragma once

#include "pdft/app/controller/controller.h"
#include "pdft/app/controller/keytree.h"
#include "pdft/app/state/document_storage.h"
#include "pdft/app/state/ui_state.h"

#include <vector>

namespace pdft::app::controller::impl {

class Controller : public controller::Controller {
 public:
    enum class Command {
        Quit,
        Reload,
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
        InvertColors,
        IncreaseScalingFactor,
        DecreaseScalingFactor,
        ToggleSidebar,
    };

    struct KeyMapping {
        Command command;
        std::string keys;
    };

    struct Config {
        std::vector<KeyMapping> key_mappings;
        int32_t rows_per_scroll = 10;
    };

    Controller(
        const Config& config,
        std::shared_ptr<state::UIState> state,
        std::shared_ptr<state::DocumentStorage> docs);

    Action onKeyEvent(char key) override;
    Action onFilesystemEvent(const notify::Event& event) override;

 private:
    void bindKey(Command command, std::string_view keys, const Config& config);
    void reloadDocument(state::DocumentId id, std::filesystem::path path);

    KeyTree key_tree_;
    std::shared_ptr<state::UIState> state_;
    std::shared_ptr<state::DocumentStorage> docs_;
    bool quit_ = false;
};

}  // namespace pdft::controller::app
