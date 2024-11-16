#include "pdft/controller/app/controller.h"

#include "pdft/core/log/log.h"

namespace pdft::controller::app {

Action Controller::onKeyEvent(char key) {
    core::log::global()->info("controller: key={}", key);

    switch (key) {
        using enum Action;

        case 'q':
            return Quit;

        case 'j':
            state_->scrollNext();
            return None;

        case 'k':
            state_->scrollPrev();
            return None;

        case 's':
            state_->nextLayout();
            return ClearScreen;

        case 'S':
            state_->prevLayout();
            return ClearScreen;

        case 'l':
            state_->nextDocument();
            return ClearScreen;

        case 'h':
            state_->prevDocument();
            return ClearScreen;

        case '=':
            state_->zoomIn();
            return ClearScreen;

        case '-':
            state_->zoomOut();
            return ClearScreen;

        default:
            return None;
    }
}

Action Controller::onFilesystemEvent(const notify::Event& event) {
    auto old_id = docs_->getIdByPath(event.path);

    core::log::global()->info(
        "controller: fs event on path={}, old_id={}", event.path.c_str(), old_id);

    docs_->removeDocument(old_id);
    auto new_id = docs_->addDocument(media::pdf::Document::open(docs_->getContext(), event.path));

    core::log::global()->info(
        "controller: replaced document with path={} old_id={}, new_id={}",
        event.path.c_str(),
        old_id,
        new_id);

    state_->onDocumentChanged(old_id, new_id);
    return Action::ClearScreen;
}

}  // namespace pdft::controller::app
