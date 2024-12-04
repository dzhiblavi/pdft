#include "pdft/app/controller/impl/controller.h"

#include "pdft/core/assert.h"
#include "pdft/core/log.h"

#include "pdft/core/result/coro/maybe.h"
#include "pdft/core/result/coro/result.h"

namespace pdft::app {

AppController::AppController(const Config& config, Arc<const media::Loader> doc_loader)
    : config_(config)
    , doc_loader_(std::move(doc_loader)) {
    for (const auto& [command, keys] : config.key_mappings) {
        bindKey(command, keys);
    }
}

void AppController::bindKey(Command command, std::string_view keys) {
    switch (command) {
        using enum Command;

        case Quit:
        case Reload:
        case CloseDocument:
        case ScrollNext:
        case ScrollPrev:
        case GoToFirstPage:
        case GoToLastPage:
        case NextDocument:
        case PrevDocument:
        case NextLayout:
        case PrevLayout:
        case InvertColors:
        case IncreaseScalingFactor:
        case DecreaseScalingFactor:
        case ToggleSidebar:
        case ToggleBold:
        case TogglePalette:
            key_tree_.bind(keys, commandCallback(command));
            break;

        case NumGoToPage:
        case NumNextPage:
        case NumPrevPage:
        case NumNextDocument:
        case NumPrevDocument:
            key_tree_.bind(KeyTree::number, keys, numCommandCallback(command));
            break;
    }
}

KeyTree::CallbackType AppController::commandCallback(Command command) {
    return [this, command] {
        command_ = {
            .count = 0,
            .command = command,
        };
    };
}

KeyTree::NumberCallbackType AppController::numCommandCallback(Command command) {
    return [this, command](uint32_t number) {
        command_ = {
            .count = number,
            .command = command,
        };
    };
}

Result<ControllerAction> AppController::applyPendingCommand(
    UIState& ui, DocumentStorage& documents) {
    using namespace result;
    auto [n, command] = command_;

    switch (command) {
        using enum Command;

        case Quit:
            return ControllerAction::Quit;

        case Reload:
            return reloadCurrentDocument(ui, documents) |
                   map([] { return ControllerAction::Redraw; });

        case CloseDocument:
            if (auto id = ui.getCurrentDocumentId()) {
                ui.removeDocument(*id);
                PDFT_VERIFY(documents.removeDocument(*id));
            }
            return ControllerAction::Redraw;

        case ScrollNext:
            ui.scrollNext(config_.rows_per_scroll);
            return ControllerAction::Redraw;

        case ScrollPrev:
            ui.scrollNext(-config_.rows_per_scroll);
            return ControllerAction::Redraw;

        case NumNextPage:
            ui.nextPage(n);
            return ControllerAction::Redraw;

        case NumPrevPage:
            ui.nextPage(-n);
            return ControllerAction::Redraw;

        case GoToFirstPage:
            ui.setPageIndex(0);
            return ControllerAction::Redraw;

        case GoToLastPage:
            ui.setPageIndex(INT_MAX);
            return ControllerAction::Redraw;

        case NumGoToPage:
            if (n > 0) {
                ui.setPageIndex(n - 1);
            }
            return ControllerAction::Redraw;

        case NextDocument:
            ui.nextDocument(+1);
            return ControllerAction::Redraw;

        case PrevDocument:
            ui.nextDocument(-1);
            return ControllerAction::Redraw;

        case NumNextDocument:
            ui.nextDocument(+n);
            return ControllerAction::Redraw;

        case NumPrevDocument:
            ui.nextDocument(-n);
            return ControllerAction::Redraw;

        case NextLayout:
            ui.nextLayout(1);
            return ControllerAction::Redraw;

        case PrevLayout:
            ui.nextLayout(-1);
            return ControllerAction::Redraw;

        case InvertColors:
            ui.invertColors();
            return ControllerAction::Redraw;

        case IncreaseScalingFactor:
            ui.updateScalingFactor(1.05f);
            return ControllerAction::Redraw;

        case DecreaseScalingFactor:
            ui.updateScalingFactor(1.0f / 1.05f);
            return ControllerAction::Redraw;

        case ToggleSidebar:
            ui.toggleSidebar();
            return ControllerAction::Redraw;

        case ToggleBold:
            ui.toggleBold();
            return ControllerAction::Redraw;

        case TogglePalette:
            ui.togglePalette();
            return ControllerAction::Redraw;
    }
}

Result<ControllerAction> AppController::onKeyEvent(
    char key, UIState& ui, DocumentStorage& documents) {
    log::debug("controller: key={}", key);

    switch (key_tree_.feed(key)) {
        using enum KeyTree::Result;

        case Accepted:
            ui.onInput(key);
            return ControllerAction::None;

        case Rejected:
            key_tree_.reset();
            ui.resetInput();
            return ControllerAction::None;

        case Terminated:
            ui.resetInput();
            return applyPendingCommand(ui, documents);
    }
}

Result<ControllerAction> AppController::onFilesystemEvent(
    const notify::Event& event, UIState& ui, DocumentStorage& documents) {
    using namespace result;
    log::debug("controller: fs event on path={}", event.path);

    auto id = co_await (
        documents.getIdByPath(event.path) | lift(makeErrorCode(ControllerError::UnkownFilePath)));

    co_await reloadDocument(id, event.path, ui, documents);
    co_return ControllerAction::Redraw;
}

Status AppController::reloadCurrentDocument(UIState& ui, DocumentStorage& documents) {
    using namespace result;

    if (auto id = ui.getCurrentDocumentId()) {
        auto document = documents.getDocument(*id) | unwrap;
        co_await reloadDocument(*id, document->getPath(), ui, documents);
    }
}

Status AppController::reloadDocument(
    DocumentId old_id, std::filesystem::path path, UIState& ui, DocumentStorage& documents) {
    using namespace result;

    PDFT_VERIFY(documents.removeDocument(old_id));
    log::debug("controller: removed document with id={}", old_id);

    auto document =
        co_await (doc_loader_->load(path) | mapErr([&](auto error) {
                      log::warn("failed to reload document by path {}: {}", path, error);
                      ui.removeDocument(old_id);
                      return error;
                  }));

    auto page_count = document->getPageCount();
    auto new_id = documents.addDocument(document);
    ui.onDocumentChanged(old_id, new_id, page_count);

    log::info("replaced document with path={} old_id={}, new_id={}", path, old_id, new_id);
}

}  // namespace pdft::app
