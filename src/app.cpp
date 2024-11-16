#include "pdft/core/io/posix/console.h"
#include "pdft/core/io/posix/termctl.h"
#include "pdft/core/log/config.h"
#include "pdft/core/log/load.h"
#include "pdft/core/log/log.h"
#include "pdft/core/notify/fsw/notify.h"
#include "pdft/core/terminal/ansi/terminal.h"
#include "pdft/core/terminal/tmux/apc_executor.h"
#include "pdft/core/trace/log_duration.h"
#include "pdft/core/trace/scope_tracer.h"

#include "pdft/display/kitty/display.h"

#include "pdft/controller/app/controller.h"

#include "pdft/view/app/renderer.h"
#include "pdft/view/app/state.h"

#include "pdft/media/pdf/context.h"
#include "pdft/media/pdf/document.h"
#include "pdft/media/pdf/document_storage.h"

#include "pdft/utils/visit.h"

#include <asio/co_spawn.hpp>
#include <asio/detached.hpp>
#include <asio/io_context.hpp>

extern "C" {
#include <mupdf/fitz/context.h>
}

#include <asio/awaitable.hpp>
#include <asio/experimental/channel.hpp>

#include <iostream>

namespace pdft {

namespace {

void warningCallback(void* user, const char* message) {
    core::log::logger("mupdf")->warn(message);
}

void errorCallback(void* user, const char* message) {
    core::log::logger("mupdf")->warn(message);
}

void setupLogger() {
    core::log::LogConfig log_config{
        .loggers =
            {
                {
                    "global",
                    {
                        .sinks =
                            {
                                core::log::SimpleFileSink{
                                    .path =
                                        "/Users/dzhiblavi/personal/projects/pdft-cxx/logs/app.log",
                                },
                            },
                    },
                },
                {
                    "mupdf",
                    {
                        .sinks =
                            {
                                core::log::SimpleFileSink{
                                    .path = "/Users/dzhiblavi/personal/projects/pdft-cxx/logs/"
                                            "mupdf.log",
                                },
                            },
                    },
                },
                {
                    "trace",
                    {
                        .level = spdlog::level::level_enum::trace,
                        .sinks =
                            {
                                core::log::SimpleFileSink{
                                    .path = "/Users/dzhiblavi/personal/projects/pdft-cxx/logs/"
                                            "trace.log",
                                },
                            },
                    },
                },
            },
    };

    core::log::loadConfig(log_config);
    core::log::global()->info("loggers have been successfully initialized");
}

void setupMupdfLoggers(const media::pdf::Context& context) {
    fz_set_warning_callback(context.native(), warningCallback, nullptr);
    fz_set_error_callback(context.native(), errorCallback, nullptr);

    core::log::global()->info("mupdf loggers have been successfully initialized");
}

struct KeyPressEvent {
    char key;
};

using Event = std::variant<KeyPressEvent, notify::Event>;

using EventQueue =
    asio::experimental::channel<asio::io_context::executor_type, void(asio::error_code, Event)>;

struct KeyboardConsumer {
    KeyboardConsumer(std::shared_ptr<core::io::Console> console) : console_(std::move(console)) {}

    asio::awaitable<void> consumeInto(EventQueue& queue) {
        while (!should_stop_) {
            auto key = co_await console_->read(1);
            co_await queue.async_send({}, KeyPressEvent{.key = key[0]});
        }
    }

    void stop() {
        should_stop_ = true;
    }

 private:
    std::shared_ptr<core::io::Console> console_;
    bool should_stop_ = false;
};

asio::awaitable<void> consumeFsEvents(EventQueue& queue, auto& notify) {
    while (!notify.isStopped()) {
        auto event = co_await notify.nextEvent();
        co_await queue.async_send({}, event);
    }
}

asio::awaitable<void> main(asio::io_context& context, std::span<char const*> paths) {
    auto pdf_context =
        std::make_shared<media::pdf::Context>(media::pdf::Context::Builder().build());
    pdf_context->registerDocumentHandlers();

    setupLogger();
    setupMupdfLoggers(*pdf_context);

    auto term_control = core::io::posix::TermControl();
    term_control.ioControl(core::io::ControlType::Input, {.canonical = false, .echo = false});

    auto terminal = std::make_shared<core::terminal::ansi::Terminal>();
    auto executor = std::make_shared<core::terminal::tmux::APCExecutor>();
    auto console = std::make_shared<core::io::posix::Console>(
        context, ::dup(STDIN_FILENO), ::dup(STDOUT_FILENO));

    {
        core::OutBuffer init_buffer;
        terminal->clear(init_buffer);
        co_await console->write(init_buffer.view());
    }

    auto queue = EventQueue(context, /* max_buffer_size= */ 10);
    auto doc_storage = std::make_shared<media::pdf::DocumentStorage>(pdf_context);
    auto display = std::make_shared<display::kitty::Display>(terminal, executor);
    auto renderer = std::make_shared<view::app::Renderer>(doc_storage, display);
    auto state = std::make_shared<view::app::State>(doc_storage);
    auto controller = std::make_shared<controller::app::Controller>(state, doc_storage);
    auto notify = pdft::notify::fsw::Notify(queue.get_executor());

    for (size_t i = 1; i < paths.size(); ++i) {
        auto path = std::filesystem::absolute(paths[i]);
        auto doc_id = doc_storage->addDocument(media::pdf::Document::open(*pdf_context, path));
        state->addDocument(doc_id);
        notify.add(path);
    }

    // Start consuming keyboard inputs in background
    auto keyboard_consumer = KeyboardConsumer(console);
    asio::co_spawn(context, keyboard_consumer.consumeInto(queue), asio::detached);

    // Start listening for filesystem events for the given path
    asio::co_spawn(context, consumeFsEvents(queue, notify), asio::detached);

    auto buffer = core::OutBuffer();
    auto layout = std::vector<view::Placement>();
    auto term_size = core::terminal::getTerminalDimensions();
    auto screen = view::IRect{
        .x = 0,
        .y = 0,
        .width = term_size.columns,
        .height = term_size.rows,
    };

    for (;;) {
        buffer.str("");
        buffer.clear();

        state->getLayout(screen, layout);

        terminal->clear(buffer);
        display->clear(buffer);

        {
            core::trace::ScopeTracer _(PDFT_LOGGING_TRACER("render_layout"));
            renderer->render(buffer, layout);
        }

        {
            core::trace::ScopeTracer _(PDFT_LOGGING_TRACER("frame_flush"));
            co_await console->write(buffer.view());
        }

        auto event = co_await queue.async_receive(asio::use_awaitable);

        auto action = utils::visit(
            event,
            [&](KeyPressEvent event) {
                return controller->onKeyEvent(event.key);
            },
            [&](notify::Event event) {
                return controller->onFilesystemEvent(event);
            });

        switch (action) {
            using enum controller::Action;

            case None:
                break;

            case ClearScreen:
            case Quit:
                break;
        }

        if (action == controller::Action::Quit) {
            core::trace::ScopeTracer _(PDFT_LOGGING_TRACER("clear_screen"));
            core::OutBuffer cmd_buffer;
            terminal->clear(cmd_buffer);
            co_await console->write(cmd_buffer.view());
            break;
        }
    }

    keyboard_consumer.stop();
    notify.stop();
}

}  // namespace

}  // namespace pdft

int main(int argc, const char** argv) {
    if (argc < 2) {
        std::cerr << "Usage: ./app <file>.pdf [<file>.pdf...]" << std::endl;
        return 1;
    }

    asio::io_context context(1);
    asio::co_spawn(context, pdft::main(context, std::span(argv, argc)), asio::detached);
    context.run();
    return 0;
}
