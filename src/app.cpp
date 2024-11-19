#include "pdft/core/io/posix/console.h"
#include "pdft/core/io/posix/termctl.h"
#include "pdft/core/log/config.h"
#include "pdft/core/log/load.h"
#include "pdft/core/log/log.h"
#include "pdft/core/notify/fsw/notify.h"
#include "pdft/core/terminal/ansi/terminal.h"
#include "pdft/core/terminal/tmux/apc_executor.h"
#include "pdft/core/trace/log.h"
#include "pdft/core/trace/stack_scope_tracer.h"

#include "pdft/controller/app/controller.h"

#include "pdft/view/app/renderer.h"
#include "pdft/view/image/kitty/display.h"

#include "pdft/state/app/state.h"
#include "pdft/state/document_storage.h"

#include "pdft/media/pdf/context.h"
#include "pdft/media/pdf/document.h"

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
    std::filesystem::path home = getenv("HOME");
    auto logs_dir = home / ".local/share/pdft";

    if (!std::filesystem::exists(logs_dir)) {
        std::filesystem::create_directories(logs_dir);
    }

    core::log::LogConfig log_config{
        .loggers =
            {
                {
                    "global",
                    {
                        .sinks =
                            {
                                core::log::SimpleFileSink{
                                    .path = home / ".local/share/pdft/app.log",
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
                                    .path = home / ".local/share/pdft/mupdf.log",
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
                                    .path = home / ".local/share/pdft/trace.log",
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

[[noreturn]] void sighandler(int sig) {
    core::fail(std::format("signal {}", sig).c_str());
}

asio::awaitable<void> main(asio::io_context& context, std::span<char const*> paths) {
    ::signal(SIGSEGV, sighandler);
    setupLogger();

    auto pdf_context =
        std::make_shared<media::pdf::Context>(media::pdf::Context::Builder().build());
    setupMupdfLoggers(*pdf_context);

    auto init = core::trace::getGlobalPool().getTracer(
        core::trace::stackTraceLogger<std::chrono::microseconds>(), "init");

    {
        auto _ = init.addChild("pdf_context_register_doc_handlers");
        pdf_context->registerDocumentHandlers();
    }

    auto term_control = core::io::posix::TermControl();

    {
        auto _ = init.addChild("term_ioctl");
        term_control.ioControl(core::io::ControlType::Input, {.canonical = false, .echo = false});
    }

    auto terminal = std::make_shared<core::terminal::ansi::Terminal>();
    auto executor = std::make_shared<core::terminal::tmux::APCExecutor>();

    auto console = [&] {
        auto _ = init.addChild("init_console");

        return std::make_shared<core::io::posix::Console>(
            context, ::dup(STDIN_FILENO), ::dup(STDOUT_FILENO));
    }();

    {
        auto _ = init.addChild("clear_screen");
        core::OutBuffer init_buffer;
        terminal->clear(init_buffer);
        co_await console->write(init_buffer.view());
    }

    const size_t MaxQueueBufferSize = 10;
    auto queue = EventQueue(context, MaxQueueBufferSize);
    auto doc_storage = std::make_shared<state::DocumentStorage>(pdf_context);
    auto display = std::make_shared<view::image::kitty::Display>(terminal, executor);
    auto renderer = std::make_shared<view::app::Renderer>(doc_storage, display, terminal);
    auto state = std::make_shared<state::app::State>(doc_storage, renderer);
    auto controller = std::make_shared<controller::app::Controller>(state, doc_storage);
    auto notify = pdft::notify::fsw::Notify(queue.get_executor());

    for (size_t i = 1; i < paths.size(); ++i) {
        auto iter = init.addChild("add_path");
        auto path = std::filesystem::absolute(paths[i]);

        if (!std::filesystem::exists(path)) {
            core::log::global()->info("path {} does not exist, skipping", path.c_str());
            continue;
        }

        auto doc_id = [&] {
            auto _ = iter.addChild("add_path_open_document");
            return doc_storage->addDocument(media::pdf::Document::open(*pdf_context, path));
        }();

        {
            auto _ = iter.addChild("add_path_to_state");
            state->addDocument(doc_id);
        }
        {
            auto _ = iter.addChild("add_path_to_notify");
            notify.add(path);
        }
    }

    // Start consuming keyboard inputs in background
    auto keyboard_consumer = KeyboardConsumer(console);
    asio::co_spawn(context, keyboard_consumer.consumeInto(queue), asio::detached);

    // Start listening for filesystem events for the given path
    asio::co_spawn(context, consumeFsEvents(queue, notify), asio::detached);

    auto buffer = core::OutBuffer();
    auto layout = std::vector<view::PagePlacement>();
    auto term_size = core::terminal::getTerminalDimensions();
    auto screen = view::IRect{
        .x = 0,
        .y = 0,
        .width = term_size.columns,
        .height = term_size.rows,
    };

    init.manualExit();

    for (;;) {
        core::log::global()->info("========== NEW FRAME ===========");

        auto tracer =
            core::trace::getGlobalPool().getTracer(core::trace::stackTraceLogger(), "display_one");

        core::clear(buffer);
        layout.clear();
        terminal->clear(buffer);

        {
            auto _ = tracer.addChild("render");
            state->render(screen, buffer);
        }

        {
            auto _ = tracer.addChild("flush");
            co_await console->write(buffer.view());
        }

        bool quit = co_await [&]() -> asio::awaitable<bool> {
            for (;;) {
                auto event = co_await queue.async_receive(asio::use_awaitable);

                auto action = utils::visit(
                    event,
                    [&](KeyPressEvent event) { return controller->onKeyEvent(event.key); },
                    [&](notify::Event event) { return controller->onFilesystemEvent(event); });

                using enum controller::Action;

                if (action == Quit) {
                    co_return true;
                }

                if (action == Redraw) {
                    co_return false;
                }
            }
        }();

        if (quit) {
            break;
        }
    }

    auto cleanup = core::trace::getGlobalPool().getTracer(
        core::trace::stackTraceLogger<std::chrono::microseconds>(), "cleanup");

    core::clear(buffer);
    {
        auto _ = cleanup.addChild("term_clear");
        terminal->clear(buffer);
        co_await console->write(buffer.view());
    }

    {
        auto _ = cleanup.addChild("keyboard_stop");
        keyboard_consumer.stop();
    }

    {
        auto _ = cleanup.addChild("notify_stop");
        notify.stop();
    }

    core::log::global()->info(
        "total stack tracer allocations: {}", core::trace::getGlobalPool().numAllocations());
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
