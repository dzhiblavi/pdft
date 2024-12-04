#include "pdft/tui/terminal.h"

#include "pdft/core/assert.h"

#include <stdio.h>
#include <sys/ioctl.h>
#include <unistd.h>

namespace pdft::tui {

TerminalDimensions Terminal::getDimensions() const {
    winsize sz;

    if (ioctl(STDIN_FILENO, TIOCGWINSZ, &sz) != 0) {
        panic("ioctl failed, errno={} ({})", errno, strerror(errno));
    }

    // XXX For debugging purposes
    if (sz.ws_row == 0) {
        return {
            .rows = 10,
            .columns = 10,
            .height_px = 100,
            .width_px = 100,
            .cell_height_px = 10,
            .cell_width_px = 10,
        };
    }

    // PDFT_VERIFY(sz.ws_row > 0 && sz.ws_col > 0, "terminal does not support ioctl TIOCGWINSZ
    // fully"); PDFT_VERIFY( sz.ws_ypixel > 0 && sz.ws_xpixel > 0, "terminal does not support ioctl
    // TIOCGWINSZ fully");

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
