#include "pdft/tui/terminal.h"

#include "pdft/core/assert.h"

#include <stdio.h>
#include <sys/ioctl.h>
#include <unistd.h>

namespace pdft::tui {

TerminalDimensions Terminal::getDimensions() const {
    winsize sz{};

    if (ioctl(STDIN_FILENO, TIOCGWINSZ, &sz) != 0) {
        panic("ioctl failed, errno={} ({})", errno, strerror(errno));
    }

    // XXX For debugging purposes
    if (sz.ws_row == 0) {
        return {
            .rows = 10,            // NOLINT
            .columns = 10,         // NOLINT
            .height_px = 100,      // NOLINT
            .width_px = 100,       // NOLINT
            .cell_height_px = 10,  // NOLINT
            .cell_width_px = 10,   // NOLINT
        };
    }

    return {
        .rows = sz.ws_row,
        .columns = sz.ws_col,
        .height_px = sz.ws_ypixel,
        .width_px = sz.ws_xpixel,
        .cell_height_px = uint32_t(sz.ws_ypixel / sz.ws_row),
        .cell_width_px = uint32_t(sz.ws_xpixel / sz.ws_col),
    };
}

}  // namespace pdft::tui
