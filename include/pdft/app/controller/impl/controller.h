#pragma once

#include "pdft/app/controller/controller.h"
#include "pdft/app/controller/keytree.h"
#include "pdft/app/state/document_storage.h"

#include "pdft/media/loader.h"

#include <vector>

namespace pdft::app::controller::impl {

class Controller : public controller::Controller {
 public:
    enum class Command {
        Quit,
        Reload,
        CloseDocument,
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

    Controller(const Config& config, std::shared_ptr<const media::Loader> doc_loader);

    Action onKeyEvent(char key, state::UIState& ui, state::DocumentStorage& documents) override;
    Action onFilesystemEvent(
        const notify::Event& event, state::UIState& ui, state::DocumentStorage& documents) override;

 private:
    Action applyPendingCommand(state::UIState& ui, state::DocumentStorage& documents);
    void bindKey(Command command, std::string_view keys);
    void reloadDocument(
        state::DocumentId id,
        std::filesystem::path path,
        state::UIState& ui,
        state::DocumentStorage& documents);

    KeyTree::CallbackType commandCallback(Command command);
    KeyTree::NumberCallbackType numCommandCallback(Command command);

    struct PendingCommand {
        uint32_t count;
        Command command;
    };

    Config config_;
    KeyTree key_tree_;
    PendingCommand command_;
    std::shared_ptr<const media::Loader> doc_loader_;
};

}  // namespace pdft::app::controller::impl
