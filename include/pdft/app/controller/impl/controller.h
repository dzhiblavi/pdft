#pragma once

#include "pdft/app/controller/controller.h"
#include "pdft/app/controller/impl/keytree.h"
#include "pdft/app/state/document_storage.h"

#include "pdft/media/loader.h"

#include <vector>

namespace pdft::app {

class AppController : public Controller {
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
        ToggleBold,
        TogglePalette,
    };

    struct KeyMapping {
        Command command;
        std::string keys;
    };

    struct Config {
        std::vector<KeyMapping> key_mappings;
        int32_t rows_per_scroll = 10;
    };

    AppController(const Config& config, Arc<const media::Loader> doc_loader);

    Result<ControllerAction> onKeyEvent(char key, UIState& ui, DocumentStorage& documents) override;

    Result<ControllerAction> onFilesystemEvent(
        const notify::Event& event, UIState& ui, DocumentStorage& documents) override;

 private:
    void bindKey(Command command, std::string_view keys);
    Result<ControllerAction> applyPendingCommand(UIState& ui, DocumentStorage& documents);
    Status reloadCurrentDocument(UIState& ui, DocumentStorage& documents);
    Status reloadDocument(
        DocumentId id, std::filesystem::path path, UIState& ui, DocumentStorage& documents);

    KeyTree::CallbackType commandCallback(Command command);
    KeyTree::NumberCallbackType numCommandCallback(Command command);

    struct PendingCommand {
        uint32_t count;
        Command command;
    };

    Config config_;
    KeyTree key_tree_;
    PendingCommand command_;
    Arc<const media::Loader> doc_loader_;
};

}  // namespace pdft::app
