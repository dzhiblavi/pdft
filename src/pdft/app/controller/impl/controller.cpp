#include "pdft/app/controller/impl/controller.h"

#include "pdft/core/log/log.h"
#include "pdft/media/mupdf/document.h"

namespace pdft::app::controller::impl {

Controller::Controller(
    const Config& config,
    std::shared_ptr<state::UIState> state,
    std::shared_ptr<state::DocumentStorage> docs)
    : state_(std::move(state))
    , docs_(std::move(docs)) {
    for (const auto& [command, keys] : config.key_mappings) {
        bindKey(command, keys, config);
    }
}

void Controller::bindKey(Command command, std::string_view keys, const Config& config) {
    switch (command) {
        using enum Command;

        case Quit:
            key_tree_.bind(keys, [this] { quit_ = true; });
            break;

        case Reload:
            key_tree_.bind(keys, [this] {
                if (auto id = state_->getCurrentDocumentId()) {
                    reloadDocument(*id, docs_->getDocument(*id).getPath());
                }
            });
            break;

        case ScrollNext:
            key_tree_.bind(keys, [&] { state_->scrollNext(config.rows_per_scroll); });
            break;

        case ScrollPrev:
            key_tree_.bind(keys, [&] { state_->scrollNext(-config.rows_per_scroll); });
            break;

        case NumNextPage:
            key_tree_.bind(KeyTree::number, keys, [this](auto times) { state_->nextPage(times); });
            break;

        case NumPrevPage:
            key_tree_.bind(KeyTree::number, keys, [this](auto times) { state_->nextPage(-times); });
            break;

        case GoToFirstPage:
            key_tree_.bind(keys, [this] { state_->setPageIndex(0); });
            break;

        case GoToLastPage:
            key_tree_.bind(keys, [this] { state_->setPageIndex(INT_MAX); });
            break;

        case NumGoToPage:
            key_tree_.bind(KeyTree::number, keys, [this](auto index) {
                if (index > 0) {
                    state_->setPageIndex(index - 1);
                }
            });
            break;

        case NextDocument:
            key_tree_.bind(keys, [this] { state_->nextDocument(+1); });
            break;

        case PrevDocument:
            key_tree_.bind(keys, [this] { state_->nextDocument(-1); });
            break;

        case NumNextDocument:
            key_tree_.bind(KeyTree::number, keys, [this](auto n) { state_->nextDocument(+n); });
            break;

        case NumPrevDocument:
            key_tree_.bind(KeyTree::number, keys, [this](auto n) { state_->nextDocument(-n); });
            break;

        case NextLayout:
            key_tree_.bind(keys, [this] { state_->nextLayout(1); });
            break;

        case PrevLayout:
            key_tree_.bind(keys, [this] { state_->nextLayout(-1); });
            break;

        case InvertColors:
            key_tree_.bind(keys, [this] { state_->invertColors(); });
            break;

        case IncreaseScalingFactor:
            key_tree_.bind(keys, [this] { state_->updateScalingFactor(1.05f); });
            break;

        case DecreaseScalingFactor:
            key_tree_.bind(keys, [this] { state_->updateScalingFactor(1.0f / 1.05f); });
            break;

        case ToggleSidebar:
            key_tree_.bind(keys, [this] { state_->toggleSidebar(); });
            break;
    }
}

Action Controller::onKeyEvent(char key) {
    log::debug("controller: key={}", key);

    switch (key_tree_.feed(key)) {
        using enum KeyTree::Result;

        case Accepted:
            state_->onInput(key);
            return Action::Redraw;

        case Rejected:
            key_tree_.reset();
            state_->resetInput();
            return Action::Redraw;

        case Terminated:
            state_->resetInput();
            return quit_ ? Action::Quit : Action::Redraw;
    }
}

Action Controller::onFilesystemEvent(const notify::Event& event) {
    log::debug("controller: fs event on path={}", event.path.c_str());

    auto old_id = docs_->getIdByPath(event.path);
    reloadDocument(old_id, event.path);
    return Action::Redraw;
}

void Controller::reloadDocument(state::DocumentId old_id, std::filesystem::path path) {
    log::debug("controller: old_id={}", old_id);

    docs_->removeDocument(old_id);
    log::debug("controller: removed document with id={}", old_id);

    auto document = media::mupdf::Document::open(media::mupdf::thisThreadContext(), path);
    auto page_count = document.getPageCount();
    auto new_id = docs_->addDocument(
        std::unique_ptr<media::Document>(new media::mupdf::Document(std::move(document))));

    log::debug("controller: added document with id={}", new_id);

    log::info(
        "controller: replaced document with path={} old_id={}, new_id={}",
        path.c_str(),
        old_id,
        new_id);

    state_->onDocumentChanged(old_id, new_id, page_count);
}

}  // namespace pdft::app::controller::impl
