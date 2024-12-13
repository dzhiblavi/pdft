#include "pdft/app/controller/impl/controller.h"

#include "pdft/core/assert.h"
#include "pdft/core/log.h"
#include "pdft/core/module/register.h"

#include "pdft/core/result/coro/maybe.h"
#include "pdft/core/result/coro/result.h"

namespace pdft::app {

AppController::AppController(
    Config config,
    inject<const media::Loader, "media_loader"> doc_loader,
    inject<UI, "ui"> ui,
    inject<DocumentStorage, "docs"> documents)
    : config_(config)
    , doc_loader_(std::move(doc_loader))
    , ui_(std::move(ui))
    , documents_(std::move(documents)) {
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

Result<ControllerAction> AppController::applyPendingCommand() {
    using namespace result;
    auto [n, command] = command_;

    switch (command) {
        using enum Command;

        case Quit:
            return ControllerAction::Quit;

        case Reload:
            return reloadCurrentDocument() | map([] { return ControllerAction::Redraw; });

        case CloseDocument:
            if (auto id = ui_->getDocumentId()) {
                ui_->removeDocument(*id);
                PDFT_VERIFY(documents_->removeDocument(*id));
            }
            return ControllerAction::Redraw;

        case ScrollNext:
            ui_->scrollNext(config_.rows_per_scroll);
            return ControllerAction::Redraw;

        case ScrollPrev:
            ui_->scrollNext(-config_.rows_per_scroll);
            return ControllerAction::Redraw;

        case NumNextPage:
            ui_->nextPage(n);
            return ControllerAction::Redraw;

        case NumPrevPage:
            ui_->nextPage(-n);
            return ControllerAction::Redraw;

        case GoToFirstPage:
            ui_->setPageIndex(0);
            return ControllerAction::Redraw;

        case GoToLastPage:
            ui_->setPageIndex(INT_MAX);
            return ControllerAction::Redraw;

        case NumGoToPage:
            if (n > 0) {
                ui_->setPageIndex(n - 1);
            }
            return ControllerAction::Redraw;

        case NextDocument:
            ui_->nextDocument(+1);
            return ControllerAction::Redraw;

        case PrevDocument:
            ui_->nextDocument(-1);
            return ControllerAction::Redraw;

        case NumNextDocument:
            ui_->nextDocument(+n);
            return ControllerAction::Redraw;

        case NumPrevDocument:
            ui_->nextDocument(-n);
            return ControllerAction::Redraw;

        case NextLayout:
            ui_->nextLayout(1);
            return ControllerAction::Redraw;

        case PrevLayout:
            ui_->nextLayout(-1);
            return ControllerAction::Redraw;

        case InvertColors:
            ui_->invertColors();
            return ControllerAction::Redraw;

        case IncreaseScalingFactor:
            ui_->updateScalingFactor(1.05f);
            return ControllerAction::Redraw;

        case DecreaseScalingFactor:
            ui_->updateScalingFactor(1.0f / 1.05f);
            return ControllerAction::Redraw;

        case ToggleSidebar:
            ui_->toggleSidebar();
            return ControllerAction::Redraw;

        case ToggleBold:
            ui_->toggleBold();
            return ControllerAction::Redraw;

        case TogglePalette:
            if (ui_->getModel().display.palette.has_value()) {
                ui_->setPalette(std::nullopt);
            } else {
                ui_->setPalette("skim");
            }

            return ControllerAction::Redraw;
    }
}

Result<ControllerAction> AppController::onKeyEvent(char key) {
    log::debug("controller: key={}", key);

    switch (key_tree_.feed(key)) {
        using enum KeyTree::Result;

        case Accepted:
            ui_->onInput(key);
            return ControllerAction::None;

        case Rejected:
            key_tree_.reset();
            ui_->resetInput();
            return ControllerAction::None;

        case Terminated:
            ui_->resetInput();
            return applyPendingCommand();
    }
}

Result<ControllerAction> AppController::onFilesystemEvent(const notify::Event& event) {
    using namespace result;
    log::debug("controller: fs event on path={}", event.path);

    auto id = co_await (documents_->getIdByPath(event.path) | lift("unknown file path"));

    co_await reloadDocument(id, event.path);
    co_return ControllerAction::Redraw;
}

Status AppController::reloadCurrentDocument() {
    using namespace result;

    if (auto id = ui_->getDocumentId()) {
        auto document = documents_->getDocument(*id) | unwrap;
        co_await reloadDocument(*id, document->getPath());
    }

    co_return unit;
}

Status AppController::reloadDocument(DocumentId old_id, std::filesystem::path path) {
    using namespace result;

    PDFT_VERIFY(documents_->removeDocument(old_id));
    log::debug("controller: removed document with id={}", old_id);

    auto document =
        co_await (doc_loader_->load(path) | mapErr([&](auto error) {
                      log::warn("failed to reload document by path {}: {}", path, error);
                      ui_->removeDocument(old_id);
                      return error;
                  }));

    auto page_count = document->getPageCount();
    auto new_id = documents_->addDocument(document);
    ui_->onDocumentChanged(old_id, new_id, page_count);

    log::info("replaced document with path={} old_id={}, new_id={}", path, old_id, new_id);
    co_return unit;
}

Result<ControllerAction> AppController::onSignal(int signal) {
    return handleSignal(signal);
}

Result<ControllerAction> AppController::handleSignal(int signal) {
    switch (signal) {
        using enum ControllerAction;

        case SIGWINCH:
            log::info(this, "received SIGWINCH: terminal resized, redrawing");
            return Redraw;

        case SIGINT:
            log::info(this, "received SIGINT: exiting");
            return Quit;

        default:
            return result::err("unknown signal");
    }
}

}  // namespace pdft::app

PDFT_REGISTER_MODULE(pdft::app::AppController);
