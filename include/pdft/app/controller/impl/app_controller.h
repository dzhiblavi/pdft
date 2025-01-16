#pragma once

#include "pdft/app/controller/controller.h"
#include "pdft/app/controller/impl/keytree.h"
#include "pdft/app/state/document_storage.h"
#include "pdft/app/state/ui.h"

#include "pdft/media/loader.h"

#include "platform/module/module.h"

#include <vector>

namespace pdft::app {

class AppController : public Controller, public platform::rt::Module {
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
        static constexpr int DefaultRowsPerScroll = 10;

        std::vector<KeyMapping> key_mappings;
        int rows_per_scroll = DefaultRowsPerScroll;
    };

    AppController(
        Config config,
        platform::Arc<const media::Loader> doc_loader,
        platform::Arc<UI> ui,
        platform::Arc<DocumentStorage> documents);

    platform::Result<ControllerAction> onKeyEvent(char key) override;
    platform::Result<ControllerAction> onFilesystemEvent(const notify::Event& event) override;
    platform::Result<ControllerAction> onSignal(int signal) override;

 private:
    void bindKey(Command command, std::string_view keys);
    platform::Result<ControllerAction> applyPendingCommand();
    platform::Status reloadCurrentDocument();
    platform::Status reloadDocument(DocumentId id, std::filesystem::path path);
    platform::Result<ControllerAction> handleSignal(int signal);

    KeyTree::CallbackType commandCallback(Command command);
    KeyTree::NumberCallbackType numCommandCallback(Command command);

    struct PendingCommand {
        int count;
        Command command;
    };

    Config config_;
    KeyTree key_tree_;
    PendingCommand command_{};
    platform::Arc<const media::Loader> doc_loader_;
    platform::Arc<UI> ui_;
    platform::Arc<DocumentStorage> documents_;
};

}  // namespace pdft::app
