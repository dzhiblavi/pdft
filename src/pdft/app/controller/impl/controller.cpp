#include "pdft/app/controller/impl/controller.h"

#include "pdft/core/assert.h"
#include "pdft/core/log/log.h"

#include "pdft/core/result/coro/maybe.h"
#include "pdft/core/result/coro/result.h"

namespace pdft::app::controller::impl {

Controller::Controller(const Config& config, Arc<const media::Loader> doc_loader)
    : config_(config)
    , doc_loader_(std::move(doc_loader)) {
    for (const auto& [command, keys] : config.key_mappings) {
        bindKey(command, keys);
    }
}

void Controller::bindKey(Command command, std::string_view keys) {
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

KeyTree::CallbackType Controller::commandCallback(Command command) {
    return [this, command] {
        command_ = {
            .count = 0,
            .command = command,
        };
    };
}

KeyTree::NumberCallbackType Controller::numCommandCallback(Command command) {
    return [this, command](uint32_t number) {
        command_ = {
            .count = number,
            .command = command,
        };
    };
}

Result<Action> Controller::applyPendingCommand(
    state::UIState& ui, state::DocumentStorage& documents) {
    using namespace result;
    auto [n, command] = command_;

    switch (command) {
        using enum Command;

        case Quit:
            return Action::Quit;

        case Reload:
            return reloadCurrentDocument(ui, documents) | map([](Unit) { return Action::Redraw; });

        case CloseDocument:
            if (auto id = ui.getCurrentDocumentId()) {
                ui.removeDocument(*id);
                PDFT_VERIFY(documents.removeDocument(*id));
            }
            return Action::Redraw;

        case ScrollNext:
            ui.scrollNext(config_.rows_per_scroll);
            return Action::Redraw;

        case ScrollPrev:
            ui.scrollNext(-config_.rows_per_scroll);
            return Action::Redraw;

        case NumNextPage:
            ui.nextPage(n);
            return Action::Redraw;

        case NumPrevPage:
            ui.nextPage(-n);
            return Action::Redraw;

        case GoToFirstPage:
            ui.setPageIndex(0);
            return Action::Redraw;

        case GoToLastPage:
            ui.setPageIndex(INT_MAX);
            return Action::Redraw;

        case NumGoToPage:
            if (n > 0) {
                ui.setPageIndex(n - 1);
            }
            return Action::Redraw;

        case NextDocument:
            ui.nextDocument(+1);
            return Action::Redraw;

        case PrevDocument:
            ui.nextDocument(-1);
            return Action::Redraw;

        case NumNextDocument:
            ui.nextDocument(+n);
            return Action::Redraw;

        case NumPrevDocument:
            ui.nextDocument(-n);
            return Action::Redraw;

        case NextLayout:
            ui.nextLayout(1);
            return Action::Redraw;

        case PrevLayout:
            ui.nextLayout(-1);
            return Action::Redraw;

        case InvertColors:
            ui.invertColors();
            return Action::Redraw;

        case IncreaseScalingFactor:
            ui.updateScalingFactor(1.05f);
            return Action::Redraw;

        case DecreaseScalingFactor:
            ui.updateScalingFactor(1.0f / 1.05f);
            return Action::Redraw;

        case ToggleSidebar:
            ui.toggleSidebar();
            return Action::Redraw;

        case TogglePalette:
            ui.togglePalette();
            return Action::Redraw;
    }
}

Result<Action> Controller::onKeyEvent(
    char key, state::UIState& ui, state::DocumentStorage& documents) {
    log::debug("controller: key={}", key);

    switch (key_tree_.feed(key)) {
        using enum KeyTree::Result;

        case Accepted:
            ui.onInput(key);
            return Action::None;

        case Rejected:
            key_tree_.reset();
            ui.resetInput();
            return Action::None;

        case Terminated:
            ui.resetInput();
            return applyPendingCommand(ui, documents);
    }
}

Result<Action> Controller::onFilesystemEvent(
    const notify::Event& event, state::UIState& ui, state::DocumentStorage& documents) {
    using namespace result;
    log::debug("controller: fs event on path={}", event.path);

    auto id = co_await (
        documents.getIdByPath(event.path) |
        lift(std::make_error_code(std::errc::io_error)));  // XXX

    co_await reloadDocument(id, event.path, ui, documents);
    co_return Action::Redraw;
}

Result<Unit> Controller::reloadCurrentDocument(
    state::UIState& ui, state::DocumentStorage& documents) {
    using namespace result;

    auto id = co_await (
        ui.getCurrentDocumentId() | lift(std::make_error_code(std::errc::io_error)));  // XXX
    auto document = documents.getDocument(id) | unwrap;

    co_return co_await reloadDocument(id, document->getPath(), ui, documents);
}

Status Controller::reloadDocument(
    state::DocumentId old_id,
    std::filesystem::path path,
    state::UIState& ui,
    state::DocumentStorage& documents) {
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
    co_return unit;
}

}  // namespace pdft::app::controller::impl
