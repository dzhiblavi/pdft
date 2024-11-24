#pragma once

#include "pdft/display/display.h"
#include "pdft/display/kitty/apc/executor.h"

#include "pdft/core/io/console.h"
#include "pdft/core/terminal/terminal.h"

#include <memory>
#include <unordered_map>

namespace pdft::display::kitty {

class DisplayBase : public display::Display {
 public:
    DisplayBase(
        std::shared_ptr<core::terminal::Terminal> terminal,
        std::shared_ptr<apc::Executor> executor,
        std::shared_ptr<core::io::Console> console)
        : terminal_(std::move(terminal))
        , executor_(std::move(executor))
        , console_(std::move(console)) {}

    asio::awaitable<void> loadImage(
        ImageId id, const Image& image, core::io::OutBuffer& buffer) override;

    void clear(core::io::OutBuffer& buffer) override;

    void unload(ImageId id, core::io::OutBuffer& buffer) override;

    void unloadAll(core::io::OutBuffer& buffer) override;

    bool isLoaded(ImageId id) const override;

 protected:
    std::shared_ptr<core::terminal::Terminal> terminal_;
    std::shared_ptr<apc::Executor> executor_;
    std::shared_ptr<core::io::Console> console_;
    std::unordered_map<ImageId, ImageSize> loaded_;
};

}  // namespace pdft::display::kitty
