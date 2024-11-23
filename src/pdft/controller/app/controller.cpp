#include "pdft/controller/app/controller.h"

#include "pdft/core/log/log.h"

namespace pdft::controller::app {

using namespace core::log::exported;

Controller::Controller(
    const Config& config,
    std::shared_ptr<state::State> state,
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

        case ZoomIn:
            key_tree_.bind(keys, [&] { state_->updateZoom(config.zoom_multiplier); });
            break;

        case ZoomOut:
            key_tree_.bind(keys, [&] { state_->updateZoom(1.f / config.zoom_multiplier); });
            break;
    }
}

Action Controller::onKeyEvent(char key) {
    log::info("controller: key={}", key);

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
    log::info("controller: fs event on path={}", event.path.c_str());

    auto old_id = docs_->getIdByPath(event.path);
    log::info("controller: old_id={}", old_id);

    docs_->removeDocument(old_id);
    log::info("controller: removed document with id={}", old_id);

    auto new_id = docs_->addDocument(media::pdf::Document::open(docs_->getContext(), event.path));
    log::info("controller: added document with id={}", new_id);

    log::info(
        "controller: replaced document with path={} old_id={}, new_id={}",
        event.path.c_str(),
        old_id,
        new_id);

    state_->onDocumentChanged(old_id, new_id);
    return Action::Redraw;
}

}  // namespace pdft::controller::app
