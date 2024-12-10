#include "pdft/core/io/posix_termctl.h"
#include "pdft/core/module/register.h"

#include <termios.h>
#include <unistd.h>

namespace pdft::io {

void PosixTermControl::ioControl(ControlType type, const IoControl& control) {
    struct termios term_old;
    tcgetattr(STDIN_FILENO, &term_old);
    struct termios newt = term_old;

    if (control.canonical.has_value()) {
        if (*control.canonical) {
            newt.c_lflag &= ICANON;
        } else {
            newt.c_lflag &= ~ICANON;
        }
    }

    if (control.echo.has_value()) {
        if (*control.echo) {
            newt.c_lflag &= ECHO;
        } else {
            newt.c_lflag &= ~ECHO;
        }
    }

    auto fileno = type == ControlType::Input ? STDIN_FILENO : STDOUT_FILENO;
    tcsetattr(fileno, TCSANOW, &newt);
}

}  // namespace pdft::io

PDFT_REGISTER_MODULE(pdft::io::PosixTermControl);
