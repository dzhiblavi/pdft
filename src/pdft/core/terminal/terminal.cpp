#include "pdft/core/terminal/terminal.h"

#include <stdio.h>
#include <sys/ioctl.h>

namespace pdft::core::terminal {

Dimensions getTerminalDimensions() {
    winsize sz;
    ioctl(0, TIOCGWINSZ, &sz);

    return {
        .rows = sz.ws_row,
        .columns = sz.ws_col,
        .height_px = sz.ws_ypixel,
        .width_px = sz.ws_xpixel,
        .cell_height_px = uint32_t(sz.ws_ypixel / sz.ws_row),
        .cell_width_px = uint32_t(sz.ws_xpixel / sz.ws_col),
    };
}

}  // namespace pdft::core::terminal
