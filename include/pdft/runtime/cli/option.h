#pragma once

#include <tclap/CmdLine.h>

namespace pdft::rt {

struct IOption {
    virtual ~IOption() = default;
    virtual void attach(TCLAP::CmdLine& cmd) = 0;
};

}  // namespace pdft::rt::cli
