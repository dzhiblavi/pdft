#pragma once

#include "pdft/core/io/console.h"
#include "pdft/core/terminal/apc_executor.h"
#include "pdft/core/terminal/terminal.h"
#include "pdft/display/display.h"

#include <memory>
#include <unordered_map>

namespace pdft::display::kitty {

class DisplayBase : public display::Display {
 public:
    DisplayBase(
        std::shared_ptr<core::terminal::Terminal> terminal,
        std::shared_ptr<core::terminal::APCExecutor> executor,
        std::shared_ptr<core::io::Console> console)
        : terminal_(std::move(terminal))
        , executor_(std::move(executor))
        , console_(std::move(console)) {}

    asio::awaitable<void> loadImage(
        media::ImageId id, const media::Image& image, core::io::OutBuffer& buffer) override;

    bool isLoaded(media::ImageId id) const override;
    void clear(core::io::OutBuffer& buffer) const override;

 protected:
    std::shared_ptr<core::terminal::Terminal> terminal_;
    std::shared_ptr<core::terminal::APCExecutor> executor_;
    std::shared_ptr<core::io::Console> console_;
    std::unordered_map<media::ImageId, media::ImageSize> loaded_;
};

}  // namespace pdft::display::kitty
