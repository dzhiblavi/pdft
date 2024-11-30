#include "pdft/app/controller/impl/controller.h"

#include "pdft/core/assert.h"
#include "pdft/core/log/log.h"

namespace pdft::app::controller::impl {

Controller::Controller(const Config& config, std::shared_ptr<const media::Loader> doc_loader)
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

Action Controller::applyPendingCommand(state::UIState& ui, state::DocumentStorage& documents) {
    auto [n, command] = command_;

    switch (command) {
        using enum Command;

        case Quit:
            return Action::Quit;

        case Reload:
            if (auto id = ui.getCurrentDocumentId()) {
                auto maybe_document = documents.getDocument(*id);
                PDFT_VERIFY(maybe_document);
                auto document = *maybe_document;
                reloadDocument(*id, document->getPath(), ui, documents);
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
    }
}

Action Controller::onKeyEvent(char key, state::UIState& ui, state::DocumentStorage& documents) {
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

Action Controller::onFilesystemEvent(
    const notify::Event& event, state::UIState& ui, state::DocumentStorage& documents) {
    log::debug("controller: fs event on path={}", event.path.c_str());

    auto maybe_doc_id = documents.getIdByPath(event.path);
    if (!maybe_doc_id) {
        log::warn("received a filesystem event for unknown path {}", event.path.c_str());
        return Action::None;
    }

    reloadDocument(*maybe_doc_id, event.path, ui, documents);
    return Action::Redraw;
}

void Controller::reloadDocument(
    state::DocumentId old_id,
    std::filesystem::path path,
    state::UIState& ui,
    state::DocumentStorage& documents) {
    log::debug("controller: old_id={}", old_id);

    PDFT_VERIFY(documents.removeDocument(old_id));
    log::debug("controller: removed document with id={}", old_id);

    auto maybe_document = doc_loader_->load(path);
    if (!maybe_document) {
        log::warn("failed to reload document by path {}: {}", path.c_str(), maybe_document.error());
        ui.removeDocument(old_id);
        return;
    }

    auto document = *maybe_document;
    auto page_count = document->getPageCount();
    auto new_id = documents.addDocument(document);
    ui.onDocumentChanged(old_id, new_id, page_count);

    log::debug("controller: added document with id={}", new_id);
    log::info(
        "controller: replaced document with path={} old_id={}, new_id={}",
        path.c_str(),
        old_id,
        new_id);
}

}  // namespace pdft::app::controller::impl
