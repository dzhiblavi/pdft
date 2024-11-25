#include "pdft/tui/terminal/terminal.h"
#include "pdft/core/assert.h"

#include <format>
#include <stdio.h>
#include <sys/ioctl.h>
#include <unistd.h>

namespace pdft::tui::terminal {

Dimensions Terminal::getDimensions() const {
    winsize sz;

    if (ioctl(STDIN_FILENO, TIOCGWINSZ, &sz) != 0) {
        core::fail(std::format("ioctl failed, errno={} ({})", errno, strerror(errno)).c_str());
    }

    PDFT_VERIFY(sz.ws_row > 0 && sz.ws_col > 0, "terminal does not support ioctl TIOCGWINSZ fully");
    PDFT_VERIFY(
        sz.ws_ypixel > 0 && sz.ws_xpixel > 0, "terminal does not support ioctl TIOCGWINSZ fully");

    return {
        .rows = sz.ws_row,
        .columns = sz.ws_col,
        .height_px = sz.ws_ypixel,
        .width_px = sz.ws_xpixel,
        .cell_height_px = uint32_t(sz.ws_ypixel / sz.ws_row),
        .cell_width_px = uint32_t(sz.ws_xpixel / sz.ws_col),
    };
}

}  // namespace pdft::tui::terminal
